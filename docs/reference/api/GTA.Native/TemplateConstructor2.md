# Template Constructor (String, Type, Parameter[])

## Visual Basic

Overloads Public Sub New( \_
   ByVal *Name* As [String](https://learn.microsoft.com/dotnet/api/system.string), \_
   ByVal *ReturnType* As [System.Type](https://learn.microsoft.com/dotnet/api/system.type), \_
   ParamArray *Arguments* As [GTA.Native.Parameter()](Parameter.md) \_
)

## C#

public Template(
   [string](https://learn.microsoft.com/dotnet/api/system.string) *Name*,
   [System.Type](https://learn.microsoft.com/dotnet/api/system.type) *ReturnType*,
   params GTA.Native.Parameter[] *Arguments*
);

#### See Also

[Template Class](Template.md) | [GTA.Native Namespace](index.md) | [Template Constructor Overload List](TemplateConstructor.md)
