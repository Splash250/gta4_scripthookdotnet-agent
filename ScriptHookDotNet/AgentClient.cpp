/*
* Copyright (c) 2009-2011 Hazard (hazard_x@gmx.net / twitter.com/HazardX)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "stdafx.h"

#include "AgentClient.h"

#include "AgentSettings.h"

#pragma managed

namespace GTA {

	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::IO;
	using namespace System::Net;
	using namespace System::Text;
	using namespace System::Threading;
	using namespace System::Web::Script::Serialization;

	String^ AgentClient::EscapeJson(String^ value) {
		if (isNULL(value)) return String::Empty;

		StringBuilder^ sb = gcnew StringBuilder();
		for each (wchar_t ch in value) {
			switch (ch) {
				case L'\\': sb->Append("\\\\"); break;
				case L'"': sb->Append("\\\""); break;
				case L'\b': sb->Append("\\b"); break;
				case L'\f': sb->Append("\\f"); break;
				case L'\n': sb->Append("\\n"); break;
				case L'\r': sb->Append("\\r"); break;
				case L'\t': sb->Append("\\t"); break;
				default:
					if (ch < 32)
						sb->AppendFormat("\\u{0:x4}", (int)ch);
					else
						sb->Append(ch);
					break;
			}
		}
		return sb->ToString();
	}

	String^ AgentClient::ReadResponseBody(WebResponse^ response) {
		if isNULL(response) return String::Empty;
		Stream^ stream = response->GetResponseStream();
		if isNULL(stream) return String::Empty;
		StreamReader^ reader = gcnew StreamReader(stream, Encoding::UTF8);
		return reader->ReadToEnd();
	}

	String^ AgentClient::ExtractErrorText(Dictionary<String^, Object^>^ root) {
		if (isNULL(root) || !root->ContainsKey("error") || isNULL(root["error"])) return String::Empty;

		Dictionary<String^, Object^>^ error = dynamic_cast<Dictionary<String^, Object^>^>(root["error"]);
		if (isNULL(error) || !error->ContainsKey("message") || isNULL(error["message"])) return String::Empty;
		return error["message"]->ToString();
	}

	String^ AgentClient::ExtractRefusalText(Dictionary<String^, Object^>^ root) {
		if (isNULL(root) || !root->ContainsKey("output") || isNULL(root["output"])) return String::Empty;

		array<Object^>^ output = dynamic_cast<array<Object^>^>(root["output"]);
		if isNULL(output) return String::Empty;

		StringBuilder^ sb = gcnew StringBuilder();
		for each (Object^ itemObj in output) {
			Dictionary<String^, Object^>^ item = dynamic_cast<Dictionary<String^, Object^>^>(itemObj);
			if (isNULL(item) || !item->ContainsKey("content") || isNULL(item["content"])) continue;

			array<Object^>^ content = dynamic_cast<array<Object^>^>(item["content"]);
			if isNULL(content) continue;

			for each (Object^ contentObj in content) {
				Dictionary<String^, Object^>^ contentItem = dynamic_cast<Dictionary<String^, Object^>^>(contentObj);
				if (isNULL(contentItem)) continue;

				bool isRefusalItem =
					(contentItem->ContainsKey("type") && isNotNULL(contentItem["type"]) && (contentItem["type"]->ToString() == "refusal")) ||
					(contentItem->ContainsKey("refusal") && isNotNULL(contentItem["refusal"]));
				if (!isRefusalItem) continue;

				String^ refusalText = contentItem->ContainsKey("refusal") && isNotNULL(contentItem["refusal"])
					? contentItem["refusal"]->ToString()
					: String::Empty;
				if (String::IsNullOrEmpty(refusalText)) continue;
				if (sb->Length > 0) sb->Append(Environment::NewLine);
				sb->Append(refusalText);
			}
		}

		return sb->ToString();
	}

	String^ AgentClient::ExtractOutputText(Dictionary<String^, Object^>^ root) {
		if isNULL(root) return String::Empty;

		if (root->ContainsKey("output_text") && isNotNULL(root["output_text"])) {
			return root["output_text"]->ToString();
		}

		if (!root->ContainsKey("output") || isNULL(root["output"])) return String::Empty;
		array<Object^>^ output = dynamic_cast<array<Object^>^>(root["output"]);
		if isNULL(output) return String::Empty;

		StringBuilder^ sb = gcnew StringBuilder();
		for each (Object^ itemObj in output) {
			Dictionary<String^, Object^>^ item = dynamic_cast<Dictionary<String^, Object^>^>(itemObj);
			if (isNULL(item) || !item->ContainsKey("content") || isNULL(item["content"])) continue;

			array<Object^>^ content = dynamic_cast<array<Object^>^>(item["content"]);
			if isNULL(content) continue;

			for each (Object^ contentObj in content) {
				Dictionary<String^, Object^>^ contentItem = dynamic_cast<Dictionary<String^, Object^>^>(contentObj);
				if (isNULL(contentItem) || !contentItem->ContainsKey("text") || isNULL(contentItem["text"])) continue;
				if (sb->Length > 0) sb->Append(Environment::NewLine);
				sb->Append(contentItem["text"]->ToString());
			}
		}
		return sb->ToString();
	}

	AgentResponse^ AgentClient::ParseResponse(String^ json) {
		AgentResponse^ result = gcnew AgentResponse();
		if (String::IsNullOrEmpty(json)) {
			result->Error = "OpenAI returned an empty response.";
			return result;
		}

		try {
			JavaScriptSerializer^ serializer = gcnew JavaScriptSerializer();
			serializer->MaxJsonLength = Int32::MaxValue;
			Dictionary<String^, Object^>^ root = dynamic_cast<Dictionary<String^, Object^>^>(serializer->DeserializeObject(json));
			if isNULL(root) {
				result->Error = "OpenAI returned an unreadable response.";
				return result;
			}

			if (root->ContainsKey("id") && isNotNULL(root["id"])) {
				result->ResponseId = root["id"]->ToString();
			}

			result->Error = ExtractErrorText(root);
			if (result->Error->Length == 0) {
				String^ refusal = ExtractRefusalText(root);
				if (refusal->Length > 0)
					result->Error = refusal;
			}
			result->Text = ExtractOutputText(root);

			if ((result->Text->Length == 0) && (result->Error->Length == 0)) {
				result->Error = "OpenAI returned no text output.";
			}
		} catch (Exception^ ex) {
			result->Error = "OpenAI response parse failed: " + ex->Message;
		}

		return result;
	}

	AgentResponse^ AgentClient::SendCore(String^ instructions, String^ userInput, String^ previousResponseId, String^ textFormatJson) {
		AgentResponse^ result = gcnew AgentResponse();

		String^ apiKey = AgentSettings::ApiKey->Trim();
		String^ model = AgentSettings::Model->Trim();
		String^ prompt = isNULL(instructions) ? String::Empty : instructions->Trim();
		if ((apiKey->Length == 0) || (model->Length == 0) || (prompt->Length == 0)) {
			result->Error = "agents.ini is missing required OpenAI settings.";
			return result;
		}

		try {
			ServicePointManager::SecurityProtocol = static_cast<SecurityProtocolType>(
				static_cast<int>(SecurityProtocolType::Tls12));

			StringBuilder^ body = gcnew StringBuilder();
			body->Append("{");
			body->Append("\"model\":\"")->Append(EscapeJson(model))->Append("\",");
			body->Append("\"instructions\":\"")->Append(EscapeJson(prompt))->Append("\",");
			body->Append("\"input\":[{\"role\":\"user\",\"content\":[{\"type\":\"input_text\",\"text\":\"")->Append(EscapeJson(userInput))->Append("\"}]}]");
			if (!String::IsNullOrEmpty(textFormatJson)) {
				body->Append(",\"text\":{\"format\":")->Append(textFormatJson)->Append("}");
			}
			if (!String::IsNullOrEmpty(previousResponseId)) {
				body->Append(",\"previous_response_id\":\"")->Append(EscapeJson(previousResponseId))->Append("\"");
			}
			body->Append("}");

			array<Byte>^ payload = Encoding::UTF8->GetBytes(body->ToString());
			HttpWebRequest^ request = dynamic_cast<HttpWebRequest^>(WebRequest::Create("https://api.openai.com/v1/responses"));
			request->Method = "POST";
			request->ContentType = "application/json";
			request->Accept = "application/json";
			request->Headers[HttpRequestHeader::Authorization] = "Bearer " + apiKey;
			request->Timeout = 120000;
			request->ReadWriteTimeout = 120000;
			request->ContentLength = payload->Length;

			Stream^ requestStream = request->GetRequestStream();
			requestStream->Write(payload, 0, payload->Length);
			requestStream->Close();

			HttpWebResponse^ response = dynamic_cast<HttpWebResponse^>(request->GetResponse());
			String^ bodyText = ReadResponseBody(response);
			response->Close();
			return ParseResponse(bodyText);
		} catch (WebException^ ex) {
			String^ bodyText = ReadResponseBody(ex->Response);
			if (!String::IsNullOrEmpty(bodyText)) {
				AgentResponse^ parsed = ParseResponse(bodyText);
				if (parsed->Error->Length > 0) return parsed;
			}
			result->Error = "OpenAI request failed: " + ex->Message;
			return result;
		} catch (Exception^ ex) {
			result->Error = "OpenAI request failed: " + ex->Message;
			return result;
		}
	}

	AgentResponse^ AgentClient::Send(String^ userInput, String^ previousResponseId) {
		return SendCore(AgentSettings::SystemPrompt, userInput, previousResponseId, String::Empty);
	}

	AgentResponse^ AgentClient::SendIsolated(String^ instructions, String^ userInput) {
		return SendCore(instructions, userInput, String::Empty, String::Empty);
	}

	AgentResponse^ AgentClient::SendIsolatedStructured(String^ instructions, String^ userInput, String^ textFormatJson) {
		return SendCore(instructions, userInput, String::Empty, textFormatJson);
	}

	AgentRequestWorker::AgentRequestWorker() {
		pSyncRoot = gcnew System::Object();
		bBusy = false;
		pCompletedResponse = nullptr;
	}

	bool AgentRequestWorker::IsBusy::get() {
		Monitor::Enter(pSyncRoot);
		try {
			return bBusy;
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	bool AgentRequestWorker::Submit(String^ userInput, String^ previousResponseId, bool storeResponseAsConversationState) {
		Monitor::Enter(pSyncRoot);
		try {
			if (bBusy) return false;
			bBusy = true;
			pCompletedResponse = nullptr;
		} finally {
			Monitor::Exit(pSyncRoot);
		}

		AgentRequestContext^ context = gcnew AgentRequestContext();
		context->UserInput = userInput;
		context->PreviousResponseId = previousResponseId;
		context->StoreResponseAsConversationState = storeResponseAsConversationState;

		Thread^ worker = gcnew Thread(gcnew ParameterizedThreadStart(this, &AgentRequestWorker::WorkerMain));
		worker->IsBackground = true;
		worker->Start(context);
		return true;
	}

	void AgentRequestWorker::WorkerMain(System::Object^ state) {
		AgentRequestContext^ context = safe_cast<AgentRequestContext^>(state);
		AgentResponse^ response = AgentClient::Send(context->UserInput, context->PreviousResponseId);
		if isNotNULL(response)
			response->StoreAsPreviousResponse = context->StoreResponseAsConversationState;

		Monitor::Enter(pSyncRoot);
		try {
			pCompletedResponse = response;
			bBusy = false;
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

	bool AgentRequestWorker::TryTakeCompleted([System::Runtime::InteropServices::Out] AgentResponse^% response) {
		response = nullptr;
		Monitor::Enter(pSyncRoot);
		try {
			if (isNULL(pCompletedResponse)) return false;
			response = pCompletedResponse;
			pCompletedResponse = nullptr;
			return true;
		} finally {
			Monitor::Exit(pSyncRoot);
		}
	}

}
