

using UnrealBuildTool;
using System.Collections.Generic;

public class FalseRoutesTarget : TargetRules
{
	public FalseRoutesTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;

		ExtraModuleNames.AddRange( new string[] { "FalseRoutes" } );
	}
}
