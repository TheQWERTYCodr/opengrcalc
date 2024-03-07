{ sources ? import ./nix/sources.nix }:
let
	pkgs = import sources.nixpkgs { config = { replaceStdenv = ({ pkgs }: pkgs.clangStdenv); }; overlays = []; };
in {
	shell = pkgs.mkShell {
		name="opengrcalc";
		packages = with pkgs; [
			niv
		];
	};
}
