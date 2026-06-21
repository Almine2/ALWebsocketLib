using System.IO;
using UnrealBuildTool;

public class ixwebsocket : ModuleRules
{
    public ixwebsocket(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "inc"));

        var PlatformName = Target.Platform.ToString();

        var LibFolder = Path.Combine(ModuleDirectory, "lib", PlatformName);
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.AddRange(Directory.EnumerateFiles(LibFolder, "*.lib"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            PublicAdditionalLibraries.AddRange(Directory.EnumerateFiles(LibFolder, "*.a"));
        }
    }
}