class CfgPatches {
	class tiglath_pileser {
		name = "Tiglath Pileser";
		units[] = {};
		weapons[] = {};
		requiredVersion = 1.82;
		requiredAddons[] = {"intercept_core"};
		author = "kyfohatl";
		authors[] = {"kyfohatl", "shervanator"};
		url = "https://github.com/kyfohatl/tiglath-pileser";
		version = "1.0";
		versionStr = "1.0";
		versionAr[] = {1,0};
	};
};
class Intercept {
    class kyfohatl { //Change this. It's either the name of your project if you have more than one plugin. Or just your name.
        class tiglath_pileser {
            pluginName = "tiglath-pileser";
        };
    };
};
