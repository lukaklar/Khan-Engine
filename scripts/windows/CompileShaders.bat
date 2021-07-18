if not exist "..\..\bin\shaders\vulkan\" mkdir ..\..\bin\shaders\vulkan

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -T vs_6_0 -E VS_Main ..\..\source\graphics\shaders\test.hlsl -Fo ..\..\bin\shaders\vulkan\test_VS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -T ps_6_0 -E PS_Main ..\..\source\graphics\shaders\test.hlsl -Fo ..\..\bin\shaders\vulkan\test_PS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -T vs_6_0 -E VS_Main ..\..\source\graphics\shaders\depth.hlsl -Fo ..\..\bin\shaders\vulkan\depth_VS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -T cs_6_0 -E CS_ComputeFrustums -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\tilefrustumcalculation.hlsl -Fo ..\..\bin\shaders\vulkan\tilefrustumcalculation_CS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -T cs_6_0 -E CS_CullLights -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\tileddeferredculling.hlsl -Fo ..\..\bin\shaders\vulkan\tileddeferredculling_CS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -T cs_6_0 -E CS_TiledDeferredLighting -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\tileddeferredlighting.hlsl -Fo ..\..\bin\shaders\vulkan\tileddeferredlighting_CS.spv

pause