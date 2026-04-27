#!/bin/bash

if command -v git >/dev/null 2>&1; then

	# Setup original path
	prev_path=$(pwd)

	# Move into script directory
	cd "$(dirname -- "${BASH_SOURCE[0]:-$0}")"

	# Ensure SimpleCPP is cloned (and vcpkg scripts are)
	git submodule sync --recursive
	git submodule update --init --remote --recursive

	# Run setup_vcpkg script which ensures vcpkg exists
	vcpkgScript=$(pwd)/third_party/simplecpp/scripts/setup_vcpkg.sh
	
	# Only run script if found
	if [ -f "$vcpkgScript" ]; then
		. $vcpkgScript
	else
		echo "vcpkg setup script not found at $vcpkgScript"
	fi
	
	cd $prev_path
else
	echo "git is not installed, please install git before running."
fi