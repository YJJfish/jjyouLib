/***********************************************************************
 * @file	utils.hpp
 * @author	jjyou
 * @date	2024-1-29
 * @brief	This file implements utility functions for vulkan development.
 *			This file is still under development. Some of the functions
 *			may be moved to other files in future versions.
***********************************************************************/
#ifndef jjyou_vk_utils_hpp
#define jjyou_vk_utils_hpp

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <exception>
#include <format>
#include <optional>

namespace jjyou {

	namespace vk {

		namespace utils {

			/** @name	Macros
			  * @brief	Useful macros for vulkan development.
			  */
			//@{

			#define JJYOU_VK_UTILS_CHECK(value) \
				if (VkResult err = (value); err != VK_SUCCESS) { throw ::std::runtime_error(std::format("Vulkan error in file {} line {}: {}", __FILE__, __LINE__, string_VkResult(err))); }
			
			//@}



			/** @name	Functions
			  * @brief	Useful functions for vulkan development.
			  */
			//@{

			

			/** @brief	Find suitable memory type.
			  * @param	physicalDevice	The physical device.
			  * @param	typeFilter		Memory typebit. Usually passed as VkMemoryRequirements::memoryTypeBits.
			  * @param	properties		Memory properties.
			  * @return	Memory typebit.
			  */
			inline std::optional<uint32_t> findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
				VkPhysicalDeviceMemoryProperties memProperties;
				vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
				for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
					if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
						return i;
					}
				}
				return std::nullopt;
			}

			//@}
		
		}

	}

}

#endif /* jjyou_vk_utils_hpp */