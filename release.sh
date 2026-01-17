#!/bin/bash

# Release script for Pebble Type watchface
# Usage: ./release.sh <version>
# Example: ./release.sh 1.1.0

set -e

if [ -z "$1" ]; then
  echo "Error: Version argument required"
  echo "Usage: ./release.sh <version>"
  echo "Example: ./release.sh 1.1.0"
  exit 1
fi

VERSION=$1

# Validate version format (semantic versioning)
if ! [[ $VERSION =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
  echo "Error: Invalid version format. Use semantic versioning (e.g., 1.1.0)"
  exit 1
fi

echo "Creating release for version $VERSION..."

# Check if working directory is clean
if ! git diff-index --quiet HEAD --; then
  echo "Error: Working directory is not clean. Commit or stash your changes first."
  exit 1
fi

# Create and push tag
echo "Creating tag v$VERSION..."
git tag -a "v$VERSION" -m "Release version $VERSION"

echo "Pushing tag..."
git push origin "v$VERSION"

echo "✓ Release $VERSION created successfully!"
echo "GitHub Actions will now build and create the release."
echo "Check: https://github.com/alexanderteinum/pebble-type/releases"
