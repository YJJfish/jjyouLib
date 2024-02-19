/***********************************************************************
 * @file	Swapchain.hpp
 * @author	jjyou
 * @date	2024-2-2
 * @brief	This file implements Swapchain and SwapchainBuilder class.
***********************************************************************/
#ifndef jjyou_vk_Swapchain_hpp
#define jjyou_vk_Swapchain_hpp

#include <vulkan/vulkan.h>

#include <limits>
#include <algorithm>

#include "PhysicalDevice.hpp"
#include "Device.hpp"
#include "utils.hpp"

namespace jjyou {

	namespace vk {

		//Forward declaration
		class Swapchain;
		class SwapchainBuilder;

		/***********************************************************************
		 * @class Swapchain
		 * @brief C++ wrapper for Vulkan swapchain and related objects.
		 *
		 * This class is a C++ wrapper class of Vulkan swapchain. It wraps
		 * some frequently used vulkan functions into class methods.
		 ***********************************************************************/
		class Swapchain {

		public:

			/** @brief	Default constructor.
			  */
			Swapchain(void) {}

			/** @brief	Destructor.
			  */
			~Swapchain(void) {}

			/** @brief	Check whether the wrapper class contains a VkDevice instance.
			  * @return `true` if not empty.
			  */
			bool has_value() const {
				return (this->swapchain != nullptr);
			}

			/** @brief	Call the corresponding vkDestroyXXX function to destroy the wrapped instance.
			  */
			void destroy(void) {
				if (this->swapchain != nullptr) {
					for (auto imageView : this->swapchainImageViews) {
						vkDestroyImageView(this->device, imageView, nullptr);
					}
					this->swapchainImages.clear();
					this->swapchainImageViews.clear();
					vkDestroySwapchainKHR(this->device, this->swapchain, nullptr);
					this->swapchain = nullptr;
				}
			}

			/** @brief	Get the wrapped VkSwapchainKHR instance.
			  * @return The wrapped VkSwapchainKHR instance.
			  */
			VkSwapchainKHR get() const { return this->swapchain; }

			/** @brief	Get the number of images in the swapchain.
			  * @return The number of images in the swapchain.
			  */
			std::uint32_t imageCount(void) const { return static_cast<std::uint32_t>(this->swapchainImages.size()); }

			/** @brief	Get the swapchain images.
			  * @return Vector of VkImage.
			  */
			const std::vector<VkImage>& images(void) const { return this->swapchainImages; }

			/** @brief	Get the swapchain image views.
			  * @return Vector of VkImageView.
			  */
			const std::vector<VkImageView>& imageViews(void) const { return this->swapchainImageViews; }

			/** @brief	Get the swapchain surface format.
			  * @return Swapchain surface format.
			  */
			VkSurfaceFormatKHR surfaceFormat(void) const { return this->_surfaceFormat; }

			/** @brief	Get the swapchain extent.
			  * @return Swapchain extent.
			  */
			VkExtent2D extent(void) const { return this->_extent; }

		private:

			VkSwapchainKHR swapchain = nullptr;
			VkDevice device = nullptr;
			std::vector<VkImage> swapchainImages = {};
			std::vector<VkImageView> swapchainImageViews = {};
			VkSurfaceFormatKHR _surfaceFormat = {};
			VkExtent2D _extent = {};

			friend class SwapchainBuilder;

		};

		/***********************************************************************
		 * @class SwapchainBuilder
		 * @brief Helper class to build a VkSwapchainKHR in Vulkan.
		 *
		 * This class helps to simplify the process of building a VkSwapchainKHR
		 * in vulkan.
		 ***********************************************************************/
		class SwapchainBuilder {

		public:

			/** @brief	Construct from jjyou::vk::PhysicalDevice and VkSurfaceKHR.
			  * @note	In Vulkan, VkInstance is not required as an argument to create VkDevice.
			  *			However, here we need jjyou::vk::Instance to get the enabled layers.
			  *			Device specific layers have now been deprecated, but it is still
			  *			a good idea to set them anyway to be compatible with older implementations.
			  */
			SwapchainBuilder(PhysicalDevice physicalDevice, Device device, VkSurfaceKHR surface) : physicalDevice(physicalDevice), device(device), surface(surface) {}

			/** @brief	Set preferred min image count.
			  *			By default, the min image count will be `VkSurfaceCapabilitiesKHR::minImageCount` + 1.
			  *			You can use this method to overwrite the default setting. The final min image count
			  *			passed to the swapchain creation struct will still be restricted by
			  *			`VkSurfaceCapabilitiesKHR::minImageCount` and `VkSurfaceCapabilitiesKHR::maxImageCount`.
			  *			And the actual image count of the swapchain may be larger than this value.
			  * @param	_minImageCount		Preferred min image count.
			  */
			SwapchainBuilder& preferMinImageCount(std::uint32_t _minImageCount) {
				this->preferredMinImageCount = _minImageCount;
				return *this;
			}

			/** @brief	Reset the preferred min image count to default.
			  */
			SwapchainBuilder& useDefaultMinImageCount(void) {
				this->preferredMinImageCount = 0;
				return *this;
			}

			/** @brief	Set preferred surface format.
			  *			If you have multiple preferred formats, please pass them from the most preferred one
			  *			to the least preferred one. If none of them are supported, the builder will use the
			  *			first format returned by `vkGetPhysicalDeviceSurfaceFormatsKHR`.
			  * @param	format	Preferred format.
			  */
			SwapchainBuilder& preferSurfaceFormat(VkSurfaceFormatKHR format) {
				this->preferredSurfaceFormat.push_back(format);
				return *this;
			}

			/** @brief	Set preferred present mode.
			  *			If you have multiple present modes, please pass them from the most preferred one
			  *			to the least preferred one. If none of them are supported, the builder will use
			  *			`VK_PRESENT_MODE_FIFO_KHR`.
			  * @param	presentMode		Preferred present mode.
			  */
			SwapchainBuilder& preferPresentMode(VkPresentModeKHR presentMode) {
				this->preferredPresentMode.push_back(presentMode);
				return *this;
			}

			/** @brief	Set preferred extent.
			  *			By default, the swapchain will have the same extent as the window framebuffer size,
			  *			whose values are provided by `VkSurfaceCapabilitiesKHR::currentExtent`. If they are
			  *			special values 0xFFFFFFFF, the swapchain will use `VkSurfaceCapabilitiesKHR::maxImageExtent`
			  *			as its extent.
			  *			You can use this method to overwrite the default setting. The final extent will still
			  *			be restricted by `VkSurfaceCapabilitiesKHR::minImageExtent` and
			  *			`VkSurfaceCapabilitiesKHR::maxImageExtent`.
			  * @param	extent		Preferred extent.
			  */
			SwapchainBuilder& preferExtent(VkExtent2D extent) {
				this->preferredExtent = extent;
				return *this;
			}

			/** @brief	Reset the preferred extent to default.
			  */
			SwapchainBuilder& useDefaultExtent(void) {
				this->preferredExtent.width = this->preferredExtent.height = std::numeric_limits<std::uint32_t>::max();
				return *this;
			}

			/** @brief	Set the number of image array layers.
			  *			By default, the number of image array layers is 1.
			  * @param	_imageArrayLayers	The number of image array layers.
			  */
			SwapchainBuilder& imageArrayLayers(std::uint32_t _imageArrayLayers) {
				this->_imageArrayLayers = _imageArrayLayers;
				return *this;
			}

			/** @brief	Set image usage bit.
			  *			By default, the image usage bit is `VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT`.
			  * @param	_imageUsage		Image usage bit.
			  */
			SwapchainBuilder& imageUsage(VkImageUsageFlags _imageUsage) {
				this->_imageUsage = _imageUsage;
				return *this;
			}

			/** @brief	Set alpha composition bit.
			  *			By default, the alpha composition bit is `VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR`.
			  * @param	_compositeAlpha		Alpha composition bit.
			  */
			SwapchainBuilder& compositeAlpha(VkCompositeAlphaFlagBitsKHR _compositeAlpha) {
				this->_compositeAlpha = _compositeAlpha;
				return *this;
			}

			/** @brief	Set the clip mode.
			  *			By default, it will be true.
			  * @param	_clipped		Whether to clip the swapchain.
			  */
			SwapchainBuilder& clipped(bool _clipped) {
				this->_clipped = (_clipped) ? VK_TRUE : VK_FALSE;
				return *this;
			}

			/** @brief	Set old swapchain.
			  *			If the swap chain needs to be recreated, you can use this method to specify the
			  *			reference to the old one.
			  * @param	_oldSwapchain	The old swapchain.
			  */
			SwapchainBuilder& oldSwapchain(VkSwapchainKHR _oldSwapchain) {
				this->_oldSwapchain = _oldSwapchain;
				return *this;
			}

			/** @brief	Build a VkSwapchainKHR instance.
			  * @return	A VkSwapchainKHR wrapped in jjyou::vk::Swapchain.
			  */
			Swapchain build(void) const {
				PhysicalDevice::SwapchainSupportDetails swapchainSupport = this->physicalDevice.querySwapchainSupport(this->surface);
				if (swapchainSupport.formats.empty() || swapchainSupport.presentModes.empty())
					JJYOU_VK_UTILS_THROW(VK_ERROR_FORMAT_NOT_SUPPORTED);
				VkSurfaceFormatKHR surfaceFormat = swapchainSupport.formats[0];
				for (const auto& preferredFormat : this->preferredSurfaceFormat)
					for (const auto& format : swapchainSupport.formats) {
						if (preferredFormat.format == format.format && preferredFormat.colorSpace == format.colorSpace) {
							surfaceFormat = format;
							break;
						}
				}
				VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
				for (const auto& preferredMode : this->preferredPresentMode)
					for (const auto& mode : swapchainSupport.presentModes) {
						if (preferredMode == mode) {
							presentMode = mode;
							break;
						}
					}
				VkExtent2D extent;
				if (this->preferredExtent.width != std::numeric_limits<std::uint32_t>::max() && this->preferredExtent.height != std::numeric_limits<std::uint32_t>::max()) {
					extent = this->preferredExtent;
				}
				else if (swapchainSupport.capabilities.currentExtent.width != std::numeric_limits<std::uint32_t>::max() && swapchainSupport.capabilities.currentExtent.height != std::numeric_limits<std::uint32_t>::max()) {
					extent = swapchainSupport.capabilities.currentExtent;
				}
				else {
					extent = swapchainSupport.capabilities.maxImageExtent;
				}
				extent.width = std::clamp(extent.width, swapchainSupport.capabilities.minImageExtent.width, swapchainSupport.capabilities.maxImageExtent.width);
				extent.height = std::clamp(extent.height, swapchainSupport.capabilities.minImageExtent.height, swapchainSupport.capabilities.maxImageExtent.height);
				std::uint32_t minImageCount;
				if (this->preferredMinImageCount == 0) {
					minImageCount = swapchainSupport.capabilities.minImageCount + 1;
				}
				else {
					minImageCount = this->preferredMinImageCount;
				}
				if (swapchainSupport.capabilities.maxImageCount > 0 && minImageCount > swapchainSupport.capabilities.maxImageCount) {
					minImageCount = swapchainSupport.capabilities.maxImageCount;
				}
				std::uint32_t queueFamilyIndices[2] = { *this->physicalDevice.graphicsQueueFamily(), *this->physicalDevice.presentQueueFamily() };
				VkSwapchainCreateInfoKHR createInfo{
					.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
					.pNext = nullptr,
					.flags = 0,
					.surface = this->surface,
					.minImageCount = minImageCount,
					.imageFormat = surfaceFormat.format,
					.imageColorSpace = surfaceFormat.colorSpace,
					.imageExtent = extent,
					.imageArrayLayers = this->_imageArrayLayers,
					.imageUsage = this->_imageUsage,
					.imageSharingMode = (queueFamilyIndices[0] == queueFamilyIndices[1]) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
					.queueFamilyIndexCount = (queueFamilyIndices[0] == queueFamilyIndices[1]) ? 1U : 2U,
					.pQueueFamilyIndices = queueFamilyIndices,
					.preTransform = swapchainSupport.capabilities.currentTransform,
					.compositeAlpha = this->_compositeAlpha,
					.presentMode = presentMode,
					.clipped = this->_clipped,
					.oldSwapchain = this->_oldSwapchain,
				};
				Swapchain swapchain;
				swapchain.device = this->device.get();
				JJYOU_VK_UTILS_CHECK(vkCreateSwapchainKHR(this->device.get(), &createInfo, nullptr, &swapchain.swapchain));
				std::uint32_t imageCount;
				vkGetSwapchainImagesKHR(this->device.get(), swapchain.swapchain, &imageCount, nullptr);
				swapchain.swapchainImages.resize(imageCount);
				vkGetSwapchainImagesKHR(this->device.get(), swapchain.swapchain, &imageCount, swapchain.swapchainImages.data());
				swapchain.swapchainImageViews.resize(imageCount);
				for (uint32_t i = 0; i < imageCount; ++i) {
					VkImageViewCreateInfo viewInfo{
						.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
						.pNext = nullptr,
						.flags = 0,
						.image = swapchain.swapchainImages[i],
						.viewType = VK_IMAGE_VIEW_TYPE_2D,
						.format = surfaceFormat.format,
						.components = {
							.r = VK_COMPONENT_SWIZZLE_IDENTITY,
							.g = VK_COMPONENT_SWIZZLE_IDENTITY,
							.b = VK_COMPONENT_SWIZZLE_IDENTITY,
							.a = VK_COMPONENT_SWIZZLE_IDENTITY
						},
						.subresourceRange = {
							.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
							.baseMipLevel = 0,
							.levelCount = 1,
							.baseArrayLayer = 0,
							.layerCount = 1
						}
					};
					JJYOU_VK_UTILS_CHECK(vkCreateImageView(this->device.get(), &viewInfo, nullptr, &swapchain.swapchainImageViews[i]));
				}
				swapchain._surfaceFormat = surfaceFormat;
				swapchain._extent = extent;
				return swapchain;
			}

		private:

			jjyou::vk::PhysicalDevice& physicalDevice;
			jjyou::vk::Device& device;
			VkSurfaceKHR surface;
			std::uint32_t preferredMinImageCount = 0;
			std::vector<VkSurfaceFormatKHR> preferredSurfaceFormat = {};
			std::vector<VkPresentModeKHR> preferredPresentMode = {};
			VkExtent2D preferredExtent = { .width = std::numeric_limits<std::uint32_t>::max(), .height = std::numeric_limits<std::uint32_t>::max() };
			std::uint32_t _imageArrayLayers = 1;
			VkImageUsageFlags _imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			VkCompositeAlphaFlagBitsKHR _compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			VkBool32 _clipped = VK_TRUE;
			VkSwapchainKHR _oldSwapchain = nullptr;
		};

	}

}

#endif /* jjyou_vk_Swapchain_hpp */