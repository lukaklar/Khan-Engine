if not exist "..\..\bin\shaders\vulkan\" mkdir ..\..\bin\shaders\vulkan

C:/VulkanSDK/1.2.154.1/Bin/dxc.exe -spirv -T vs_6_0 -E VS_Main ../../source/graphics/shaders/test.hlsl -Fo ../../bin/shaders/vulkan/test_VS.spv
C:/VulkanSDK/1.2.154.1/Bin/dxc.exe -spirv -T ps_6_0 -E PS_Main ../../source/graphics/shaders/test.hlsl -Fo ../../bin/shaders/vulkan/test_PS.spv
pause