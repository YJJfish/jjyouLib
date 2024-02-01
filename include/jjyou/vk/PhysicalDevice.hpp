/***********************************************************************
 * @file	PhysicalDevice.hpp
 * @author	jjyou
 * @date	2024-1-30
 * @brief	This file implements PhysicalDevice and 
 *			PhysicalDeviceSelector class.
***********************************************************************/
#ifndef jjyou_vk_PhysicalDevice_hpp
#define jjyou_vk_PhysicalDevice_hpp

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

#include "Instance.hpp"
#include "utils.hpp"

namespace jjyou {

	namespace vk {

		//Forward declaration
		class PhysicalDevice;
		class PhysicalDeviceSelector;

		/***********************************************************************
		 * @class PhysicalDevice
		 * @brief C++ wrapper for VkPhysicalDevice.
		 *
		 * This class is a C++ wrapper class of VkPhysicalDevice. It wraps
		 * some frequently used vulkan functions into class methods.
		 ***********************************************************************/
		class PhysicalDevice {

		public:

			/** @brief	Default constructor.
			  */
			PhysicalDevice(void) {}

			/** @brief	Destructor.
			  */
			~PhysicalDevice(void) {}

			/** @brief	Check whether the wrapper class contains a VkPhysicalDevice instance.
			  * @return `true` if not empty.
			  */
			bool has_value() const {
				return (this->physicalDevice != nullptr);
			}

			/** @brief	Call the corresponding vkDestroyXXX function to destroy the wrapped instance.
			  */
			void destroy(void) { }

			/** @brief	Get the wrapped VkPhysicalDevice instance.
			  * @return The wrapped VkPhysicalDevice instance.
			  */
			VkPhysicalDevice get() const { return this->physicalDevice; }

			/** @brief	Get graphics queue family index.
			  * @return Graphics queue family index.
			  */
			std::optional<std::uint32_t> graphicsQueueFamily(void) const { return this->_graphicsQueueFamily; }

			/** @brief	Get compute queue family index.
			  * @return Compute queue family index.
			  */
			std::optional<std::uint32_t> computeQueueFamily(void) const { return this->_computeQueueFamily; }

			/** @brief	Get present queue family index.
			  * @return Present queue family index.
			  */
			std::optional<std::uint32_t> presentQueueFamily(void) const { return this->_presentQueueFamily; }

			/** @brief	Get enabled device extensions.
			  * @return Vector of enabled device extensions.
			  */
			const std::vector<const char*>& enabledDeviceExtensions(void) const { return this->_enabledDeviceExtensions; }

			/** @brief	Get enabled physical device features.
			  * @return Vector of enabled physical device features.
			  */
			const VkPhysicalDeviceFeatures& enabledDeviceFeatures(void) const { return this->_enabledDeviceFeatures; }

			/** @brief Swapchain support details.
			  */
			struct SwapchainSupportDetails {
			public:
				VkSurfaceCapabilitiesKHR capabilities;
				std::vector<VkSurfaceFormatKHR> formats;
				std::vector<VkPresentModeKHR> presentModes;
			};

			/** @brief	Helper function to query swapchain support of a window surface.
			  * @param	surface			The window surface to check.
			  * @return	Swapchain support details.
			  */
			SwapchainSupportDetails querySwapchainSupport(VkSurfaceKHR surface) const {
				return PhysicalDevice::querySwapchainSupport(this->physicalDevice, surface);
			}

			/** @brief	Helper function to query swapchain support of a window surface.
			  * @param	physicalDevice	The physical device to check.
			  * @param	surface			The window surface to check.
			  * @return	Swapchain support details.
			  */
			static SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
				SwapchainSupportDetails details;
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);
				uint32_t formatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
				uint32_t presentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
				return details;
			}

			/** @brief	Wrapper function for vkEnumerateDeviceExtensionProperties.
			  * @param	pLayerName		The layer to retrieve extensions from.
			  * @return	Vector of device extension properties.
			  */
			std::vector<VkExtensionProperties> enumerateDeviceExtensionProperties(const char* pLayerName = nullptr) const {
				return PhysicalDevice::enumerateDeviceExtensionProperties(this->physicalDevice, pLayerName);
			}

			/** @brief	Wrapper function for vkEnumerateDeviceExtensionProperties.
			  * @param	physicalDevice	The physical device to retrieve extensions from.
			  * @param	pLayerName		The layer to retrieve extensions from.
			  * @return	Vector of device extension properties.
			  */
			static std::vector<VkExtensionProperties> enumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName = nullptr) {
				uint32_t extensionCount;
				vkEnumerateDeviceExtensionProperties(physicalDevice, pLayerName, &extensionCount, nullptr);
				std::vector<VkExtensionProperties> availableExtensions(extensionCount);
				vkEnumerateDeviceExtensionProperties(physicalDevice, pLayerName, &extensionCount, availableExtensions.data());
				return availableExtensions;
			}

			/** @brief	Helper function to check device extenson support.
			  * @param	extensions		The extensions to check.
			  * @param	pLayerName		The layer to retrieve extensions from.
			  * @return	`true` if all extensions are supported.
			  */
			bool checkDeviceExtensionSupport(const std::vector<const char*>& extensions, const char* pLayerName = nullptr) const {
				return PhysicalDevice::checkDeviceExtensionSupport(this->physicalDevice, extensions, pLayerName);
			}

			/** @brief	Helper function to check device extenson support.
			  * @param	physicalDevice	The physical device to retrieve extensions from.
			  * @param	extensions		The extensions to check.
			  * @param	pLayerName		The layer to retrieve extensions from.
			  * @return	`true` if all extensions are supported.
			  */
			static bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice, const std::vector<const char*>& extensions, const char* pLayerName = nullptr) {
				std::vector<VkExtensionProperties> availableExtensions = PhysicalDevice::enumerateDeviceExtensionProperties(physicalDevice, pLayerName);
				for (const auto& extensionName : extensions) {
					bool extensionFound = false;
					for (const auto& extensionProperties : availableExtensions) {
						if (strcmp(extensionName, extensionProperties.extensionName) == 0) {
							extensionFound = true;
							break;
						}
					}
					if (!extensionFound) {
						return false;
					}
				}
				return true;
			}

			/** @brief	Wrapper function for vkGetPhysicalDeviceQueueFamilyProperties.
			  * @return	Vector of VkQueueFamilyProperties.
			  */
			std::vector<VkQueueFamilyProperties> getPhysicalDeviceQueueFamilyProperties(void) const {
				return PhysicalDevice::getPhysicalDeviceQueueFamilyProperties(this->physicalDevice);
			}

			/** @brief	Wrapper function for vkGetPhysicalDeviceQueueFamilyProperties.
			  * @param	physicalDevice	VkPhysicalDevice instance.
			  * @return	Vector of VkQueueFamilyProperties.
			  */
			static std::vector<VkQueueFamilyProperties> getPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice) {
				uint32_t queueFamilyCount = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
				std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
				return queueFamilies;
			}

		private:

			VkPhysicalDevice physicalDevice = nullptr;
			std::optional<std::uint32_t> _graphicsQueueFamily = std::nullopt;
			std::optional<std::uint32_t> _computeQueueFamily = std::nullopt;
			std::optional<std::uint32_t> _presentQueueFamily = std::nullopt;
			std::vector<const char*> _enabledDeviceExtensions = {};
			VkPhysicalDeviceFeatures _enabledDeviceFeatures = {};

			friend class PhysicalDeviceSelector;

		};

		/***********************************************************************
		 * @class PhysicalDeviceSelector
		 * @brief Helper class to select a VkPhysicalDevice in Vulkan.
		 *
		 * This class helps to simplify the process of selecting a physical
		 * device in vulkan.
		 ***********************************************************************/
		class PhysicalDeviceSelector {

		public:

			/** @brief	Construct from jjyou::vk::Instance.
			  */
			PhysicalDeviceSelector(Instance instance) : instance(instance) {}

			/** @brief	Set the window surface for onscreen rendering.
			  * @note	If you created the Vulkan instance for onscreen rendering,
			  *			you MUST call this function to set the window surface.
			  * @param	_surface	The window surface.
			  */
			PhysicalDeviceSelector& surface(VkSurfaceKHR _surface) {
				this->_surface = _surface;
				return *this;
			}

			/** @brief	Enable an device extension.
			  * @note	If you created the Vulkan instance for onscreen rendering,
			  *			the "VK_KHR_swapchain" extension will be automatically added.
			  *			DO NOT call this method to add it again.
			  * @param	extensionName	The name of the extension.
			  */
			PhysicalDeviceSelector& enableDeviceExtension(const char* extensionName) {
				this->enabledDeviceExtensions.push_back(extensionName);
				return *this;
			}

			/** @brief	Request the physical device to be a dedicated graphics card. If not found,
			  *			the selected physical device will still be an integrated one.
			  * @param	request	Whether to request.
			  */
			PhysicalDeviceSelector& requestDedicated(bool request = true) {
				this->_requestDedicated = request;
				return *this;
			}

			/** @brief	Require the physical device to be a dedicated graphics card.
			  * @param	required	Whether to require.
			  */
			PhysicalDeviceSelector& requireDedicated(bool require = true) {
				this->_requireDedicated = require;
				return *this;
			}

			/** @brief	Require the physical device to have graphics queue families.
			  *			By default, this is set to `true`.
			  * @param	required	Whether to require.
			  */
			PhysicalDeviceSelector& requireGraphicsQueue(bool require = true) {
				this->_requireGraphicsQueue = require;
				return *this;
			}

			/** @brief	Require the physical device to have compute queue families.
			  *			By default, this is set to `false`.
			  * @param	required	Whether to require.
			  */
			PhysicalDeviceSelector& requireComputeQueue(bool require = true) {
				this->_requireComputeQueue = require;
				return *this;
			}

			/** @brief	Enable physical device features. Physical devices that cannot support all
			  *			the required features will not be selected.
			  * @param	features	Features to be enabled.
			  */
			PhysicalDeviceSelector& enableDeviceFeatures(const VkPhysicalDeviceFeatures& features) {
				this->enabledDeviceFeatures = features;
				return *this;
			}

			/** @brief	Select a VkPhysicalDevice instance according to the settings.
			  * @return	A VkPhysicalDevice wrapped in jjyou::vk::PhysicalDevice.
			  */
			PhysicalDevice select(void) const {
				std::vector<const char*> enabledDeviceExtensions = this->enabledDeviceExtensions;
				if (!this->instance.offscreen()) {
					enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
				}
				std::vector<VkPhysicalDevice> availablePhysicalDevices = this->instance.enumeratePhysicalDevices();
				PhysicalDevice candidateIntegratedPhysicalDevices;
				for (VkPhysicalDevice physicalDevice : availablePhysicalDevices) {
					// Check device extension support
					if (!PhysicalDevice::checkDeviceExtensionSupport(physicalDevice, enabledDeviceExtensions))
						continue;
					// Check swapchain support
					if (!this->instance.offscreen()) {
						PhysicalDevice::SwapchainSupportDetails swapchainSupport = PhysicalDevice::querySwapchainSupport(physicalDevice, this->_surface);
						if (swapchainSupport.formats.empty() || swapchainSupport.presentModes.empty())
							continue;
					}
					// Find queue families
					std::optional<std::uint32_t> presentFamily, graphicsFamily, computeFamily;
					if (!this->instance.offscreen() || this->_requireGraphicsQueue || this->_requireComputeQueue) {
						std::vector<VkQueueFamilyProperties> queueFamilies = PhysicalDevice::getPhysicalDeviceQueueFamilyProperties(physicalDevice);
						for (int i = 0; i < queueFamilies.size(); ++i) {
							const VkQueueFamilyProperties& queueFamily = queueFamilies[i];
							// Graphics
							if (this->_requireGraphicsQueue && !graphicsFamily.has_value() && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
								graphicsFamily = i;
							// Compute
							if (this->_requireComputeQueue && !computeFamily.has_value() && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))
								computeFamily = i;
							// Present
							if (!this->instance.offscreen() && !presentFamily.has_value()) {
								VkBool32 presentSupport = false;
								vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, this->_surface, &presentSupport);
								if (presentSupport)
									presentFamily = i;
							}
							if ((!this->_requireGraphicsQueue || graphicsFamily.has_value()) &&
								(!this->_requireComputeQueue || computeFamily.has_value()) &&
								(this->instance.offscreen() || presentFamily.has_value()))
								break;
						}
					}
					if ((this->_requireGraphicsQueue && !graphicsFamily.has_value()) ||
						(this->_requireComputeQueue && !computeFamily.has_value()) ||
						(!this->instance.offscreen() && !presentFamily.has_value()))
						continue;
					// Check device properties and features
					if (this->_requireDedicated || this->_requestDedicated) {
						VkPhysicalDeviceProperties deviceProperties;
						VkPhysicalDeviceFeatures deviceFeatures;
						vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
						vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
						bool allFeaturesSupported = true;
						for (int i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i) {
							if (*(reinterpret_cast<const VkBool32*>(&this->enabledDeviceFeatures) + i) && !*(reinterpret_cast<const VkBool32*>(&deviceFeatures) + i)) {
								allFeaturesSupported = false;
								break;
							}
						}
						if (!allFeaturesSupported)
							continue;
						if (this->_requireDedicated && deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
							continue;
						if (this->_requestDedicated && deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && !candidateIntegratedPhysicalDevices.has_value()) {
							candidateIntegratedPhysicalDevices.physicalDevice = physicalDevice;
							candidateIntegratedPhysicalDevices._graphicsQueueFamily = graphicsFamily;
							candidateIntegratedPhysicalDevices._computeQueueFamily = computeFamily;
							candidateIntegratedPhysicalDevices._presentQueueFamily = presentFamily;
							candidateIntegratedPhysicalDevices._enabledDeviceExtensions = enabledDeviceExtensions;
							candidateIntegratedPhysicalDevices._enabledDeviceFeatures = this->enabledDeviceFeatures;
							continue;
						}
					}
					PhysicalDevice _physicalDevice;
					_physicalDevice.physicalDevice = physicalDevice;
					_physicalDevice._graphicsQueueFamily = graphicsFamily;
					_physicalDevice._computeQueueFamily = computeFamily;
					_physicalDevice._presentQueueFamily = presentFamily;
					_physicalDevice._enabledDeviceExtensions = enabledDeviceExtensions;
					_physicalDevice._enabledDeviceFeatures = this->enabledDeviceFeatures;
					return _physicalDevice;
				}
				if (this->_requestDedicated && candidateIntegratedPhysicalDevices.has_value())
					return candidateIntegratedPhysicalDevices;
				JJYOU_VK_UTILS_CHECK(VK_ERROR_INCOMPATIBLE_DRIVER);
			}

		private:

			Instance instance;
			VkSurfaceKHR _surface = nullptr;
			std::vector<const char*> enabledDeviceExtensions = {};
			bool _requestDedicated = true;
			bool _requireDedicated = true;
			bool _requireGraphicsQueue = true;
			bool _requireComputeQueue = false;
			VkPhysicalDeviceFeatures enabledDeviceFeatures = {};

		};

	}

}


#endif /* jjyou_vk_PhysicalDevice_hpp */