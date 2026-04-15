# Template Constructor (String, Type, Parameter[])

## Visual Basic

Overloads Public Sub New( \_
   ByVal *Name* As [String](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp), \_
   ByVal *ReturnType* As [System.Type](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemTypeClassTopic.asp), \_
   ParamArray *Arguments* As [GTA.Native.Parameter()](Parameter.md) \_
)

## C#

public Template(
   [string](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemStringClassTopic.asp) *Name*,
   [System.Type](http://msdn.microsoft.com/library/default.asp?url=/library/en-us/cpref/html/frlrfSystemTypeClassTopic.asp) *ReturnType*,
   params [GTA.Native.Parameter[]](GTA.Native.Parameter.md) *Arguments*
);

#### See Also

[Template Class](Template.md) | [GTA.Native Namespace](index.md) | [Template Constructor Overload List](TemplateConstructor.md)
