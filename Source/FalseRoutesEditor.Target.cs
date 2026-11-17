

using UnrealBuildTool;
using System.Collections.Generic;

public class FalseRoutesEditorTarget : TargetRules
{
	public FalseRoutesEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;

		ExtraModuleNames.AddRange( new string[] { "FalseRoutes" } );
	}
}
