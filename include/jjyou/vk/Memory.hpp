/***********************************************************************
 * @file	Memory.hpp
 * @author	jjyou
 * @date	2024-2-6
 * @brief	This file implements Memory and MemoryAllocator class.
***********************************************************************/
#ifndef jjyou_vk_Memory_hpp
#define jjyou_vk_Memory_hpp

#include <vulkan/vulkan.h>

#include <limits>
#include <algorithm>

#include "PhysicalDevice.hpp"
#include "Device.hpp"
#include "utils.hpp"

namespace jjyou {

	namespace vk {

		class Memory {

		public:

			/** @brief	Default constructor.
			  */
			Memory(void) {}

			/** @brief	Destructor.
			  */
			~Memory(void) {}

			/** @brief	Check whether the wrapper class contains a VkDeviceMemory instance.
			  * @return `true` if not empty.
			  */
			bool has_value() const {
				return (this->_memory != nullptr);
			}

			/** @brief	Get the wrapped VkDeviceMemory instance.
			  * @return The wrapped VkDeviceMemory instance.
			  */
			VkDeviceMemory memory() const { return this->_memory; }

			/** @brief	Get memory size.
			  * @return Memory size.
			  */
			VkDeviceSize size(void) const { return this->_size; }
			
			/** @brief	Get memory offset.
			  * @return Memory offset.
			  */
			VkDeviceSize offset(void) const { return this->_offset; }

			/** @brief	Get memory mapped host address.
			  * @return Mapped host address.
			  */
			void* mappedAddress(void) const { return this->_mappedAddress; }

		private:

			VkDeviceMemory _memory = nullptr;

			VkDeviceSize _size = 0ULL;

			VkDeviceSize _offset = 0ULL;

			void* _mappedAddress = nullptr;

			friend class MemoryAllocator;

		};

		class MemoryAllocator {

		public:

			/** @brief	Default constructor.
			  */
			MemoryAllocator(void) {}

			/** @brief	Initialize allocator.
			  * @param	device	Vulkan logical device.
			  */
			void init(const Device& device) {
				this->device = &device;
			}

			/** @brief	Destory allocator.
			  */
			void destory(void) {
				this->device = nullptr;
			}
			
			/** @brief	Allocate memory.
			  */
			VkResult allocate(const VkMemoryAllocateInfo* pAllocateInfo, Memory& memory) {
				VkResult res = vkAllocateMemory(this->device->get(), pAllocateInfo, nullptr, &memory._memory);
				if (res == VK_SUCCESS) {
					memory._offset = 0ULL;
					memory._size = pAllocateInfo->allocationSize;
				}
				return res;
			}

			/** @brief	Free memory.
			  */
			void free(Memory& memory) {
				vkFreeMemory(this->device->get(), memory._memory, nullptr);
				memory._memory = nullptr;
				memory._offset = 0ULL;
				memory._size = 0ULL;
			}

			VkResult map(Memory& memory) {
				if (!memory.has_value()) return VK_ERROR_MEMORY_MAP_FAILED;
				if (memory._mappedAddress != nullptr)
					return VK_SUCCESS;
				VkResult res = vkMapMemory(this->device->get(), memory._memory, memory._offset, memory._size, 0, &memory._mappedAddress);
				return res;
			}

			VkResult unmap(Memory& memory) {
				vkUnmapMemory(this->device->get(), memory._memory);
				memory._mappedAddress = nullptr;
				return VK_SUCCESS;
			}

		private:

			const Device* device = nullptr;

		};

	}

}

#endif /* jjyou_vk_Memory_hpp */