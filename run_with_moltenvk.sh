#!/bin/bash

# Set MoltenVK environment variables
export VK_ICD_FILENAMES=$(pwd)/MoltenVK_icd_absolute.json
export VK_LAYER_PATH=/opt/homebrew/Cellar/molten-vk/1.4.0/share/vulkan/explicit_layer.d
export VULKAN_SDK=/opt/homebrew/Cellar/molten-vk/1.4.0
export VK_DRIVER_FILES=$(pwd)/MoltenVK_icd_absolute.json

# Debug info
echo "Environment variables:"
echo "VK_ICD_FILENAMES=$VK_ICD_FILENAMES"
echo "VULKAN_SDK=$VULKAN_SDK"
echo "VK_DRIVER_FILES=$VK_DRIVER_FILES"

# Check if MoltenVK files exist
echo "Checking MoltenVK files:"
ls -la /opt/homebrew/Cellar/molten-vk/1.4.0/etc/vulkan/icd.d/MoltenVK_icd.json
ls -la /opt/homebrew/Cellar/molten-vk/1.4.0/lib/libMoltenVK.dylib

# Build and run
cmake --build build
./build/bin/MentalProject