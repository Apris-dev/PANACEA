# If you get a message about powershell execution policy, run this first
# Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser

if (Get-Command git -ErrorAction SilentlyContinue) {
    
    # Setup original path (Push-Location is the PS equivalent of saving the path)
    Push-Location $PSScriptRoot

    # Ensure SimpleCPP is cloned (and vcpkg scripts are)
    git submodule sync --recursive
    git submodule update --init --remote --recursive

    # Run setup_vcpkg script which ensures vcpkg exists
    $vcpkgScript = Join-Path (Get-Location) "third_party/simplecpp/scripts/setup_vcpkg.ps1"
    
	# Only run script if found
    if (Test-Path $vcpkgScript) {
        . $vcpkgScript
    } else {
        Write-Warning "vcpkg setup script not found at $vcpkgScript"
    }

    Pop-Location
}
else {
    Write-Host "git is not installed, please install git before running." -ForegroundColor Red
}