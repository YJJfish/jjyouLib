/***********************************************************************
 * @file	Context.hpp
 * @author	jjyou
 * @date	2024-3-16
 * @brief	This file implements Context and ContextBuilder class.
***********************************************************************/
#ifndef jjyou_vk_Context_hpp
#define jjyou_vk_Context_hpp

#include <vulkan/vulkan_raii.hpp>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <set>
#include <optional>
#include <functional>
#include <iostream>

namespace jjyou {

	namespace vk {

		class Context;
		class ContextBuilder;

		/***********************************************************************
		 * @class Context
		 * @brief Vulkan context class made up of instance, physical device,
		 *        device, and queues.
		 *
		 * This class is an abstraction layer over several Vulkan resources, and
		 * have higher level methods compared with the original Vulkan API.
		 * The context owns the underlying Vulkan resources. Therefore it is
		 * movable but not copyable. Like Vulkan RAII header, it also has a constructor
		 * that takes a std::nullptr_t value to construct an empty context.
		 * Besides these, it has no other public constructors. You have to use
		 * ContextBuilder class to create it.
		 ***********************************************************************/
		class Context {

		public:

			/***********************************************************************
			 * @enum	QueueType
			 * @brief	Enum used to determine the queue type. This is defined as
			 *			C enum instead of C++ enum class so that you can directly
			 *			use it to index into the queue array.
			 ***********************************************************************/
			enum QueueType : std::size_t {
				Main,				/**< Queue family that supports graphics, transfer and compute. Guaranteed to be available. */
				Compute,			/**< Queue family that supports compute and transfer. Must be different from the Main queue family. May be unavailable. */
				Transfer,			/**< Queue family that supports transfer. Must be different from the Main and Compute queue families. May be unavailable. */
				NumQueueTypes,		/**< Used to indicate the number of queue types. */
			};

			/** @brief	Construct an empty context.
			  */
			Context(std::nullptr_t) {}

			/** @brief	Copy constructor is disabled.
			  */
			Context(const Context&) = delete;

			/** @brief	Move constructor.
			  */
			Context(Context&& other) = default;

			/** @brief	Destructor.
			  */
			~Context(void) = default;

			/** @brief	Copy assignment is disabled.
			  */
			Context& operator=(const Context&) = delete;

			/** @brief	Move assignment.
			  */
			Context& operator=(Context&& other) noexcept {
				if (this != &other) {
					this->_context = std::move(other._context);
					this->_debugUtilsMessenger = std::move(other._debugUtilsMessenger);
					this->_instance = std::move(other._instance);
					this->_physicalDevice = std::move(other._physicalDevice);
					this->_device = std::move(other._device);
				}
				return *this;
			}

			/** @brief	Swap with another context.
			  */
			void swap(Context& other) noexcept {
				std::swap(this->_context, other._context);
				std::swap(this->_instance, other._instance);
				std::swap(this->_debugUtilsMessenger, other._debugUtilsMessenger);
				std::swap(this->_physicalDevice, other._physicalDevice);
				std::swap(this->_device, other._device);
			}

			/** @brief	Get the vulkan context.
			  */
			::vk::raii::Context& context(void) {
				return this->_context;
			}

			/** @brief	Get the vulkan context.
			  */
			const ::vk::raii::Context& context(void) const {
				return this->_context;
			}

			/** @brief	Get the vulkan instance.
			  */
			const ::vk::raii::Instance& instance(void) const {
				return this->_instance;
			}

			/** @brief	Get the vulkan debug messenger.
			  */
			const ::vk::raii::DebugUtilsMessengerEXT& debugUtilsMessenger(void) const {
				return this->_debugUtilsMessenger;
			}

			/** @brief	Get the vulkan physical device.
			  */
			const ::vk::raii::PhysicalDevice& physicalDevice(void) const {
				return this->_physicalDevice;
			}

			/** @brief	Get the vulkan device.
			  */
			const ::vk::raii::Device& device(void) const {
				return this->_device;
			}

			/** @brief	Get the queue family index.
			  */
			const std::optional<uint32_t>& queueFamilyIndex(QueueType queueType_) const {
				return this->_queueFamilyIndices[queueType_];
			}

		private:

			bool _headless = true;
			bool _validation = false;
			::vk::raii::Context _context{};
			::vk::raii::Instance _instance{ nullptr };
			std::set<std::string> _enabledLayers{};
			std::set<std::string> _enabledInstanceExtensions{};
			::vk::raii::DebugUtilsMessengerEXT _debugUtilsMessenger{ nullptr };
			::vk::raii::PhysicalDevice _physicalDevice{ nullptr };
			::vk::PhysicalDeviceFeatures _enabledDeviceFeatures{};
			std::set<std::string> _enabledDeviceExtensions{};
			std::array<std::optional<std::uint32_t>, Context::QueueType::NumQueueTypes> _queueFamilyIndices{};
			::vk::raii::Device _device{ nullptr };

			friend class ContextBuilder;

		};

		/***********************************************************************
		 * @class	PhysicalDeviceInfo
		 * @brief	A wrapper class of Vulkan physical device along with queue
		 *			family indices and supported features.
		 ***********************************************************************/
		struct PhysicalDeviceInfo {

			/** @brief	Vulkan physical device.
			  */
			::vk::raii::PhysicalDevice physicalDevice{nullptr};

			/** @brief	Physical device type.
			  */
			::vk::PhysicalDeviceType type = ::vk::PhysicalDeviceType::eOther;

			/** @brief	Physical device features that can be enabled.
			  * 
			  * This variable may not be equal to the requested or required features
			  * configured in the ContextBuilder.
			  */
			::vk::PhysicalDeviceFeatures enabledDeviceFeatures{};

			/** @brief	Device extensions that can be enabled.
			  *
			  * This variable may not be equal to the required device extensions
			  * configured in the ContextBuilder.
			  */
			std::set<std::string> enabledDeviceExtensions{};

			/** @brief	Queue family indices.
			  */
			std::array<std::optional<std::uint32_t>, Context::QueueType::NumQueueTypes> queueFamilyIndices{};
			
			/** @enum	Support
			  * @brief	Enum used to indicate the support of criteria of a physical device.
			  */
			enum class Support : std::uint32_t {
				AllSupported = 0x00000000,
				TypeNotMatched = 0x00000001,
				FeatureNotSupported = 0x00000002,
				ExtensionNotSupported = 0x00000004,
				QueueNotFound = 0x00000008,
				UserDefinedCriteriaNotMet = 0x00000010,
				Unknown = 0xFFFFFFFF,
			};

			/** @brief	Support information for requested criteria.
			  */
			Support requestedCriteria = Support::Unknown;

			/** @brief	Support information for required criteria.
			  */
			Support requiredCriteria = Support::Unknown;

			/** @brief	Copy function
			  *
			  */
		};

		/***********************************************************************
		 * @class ContextBuilder
		 * @brief Builder class to build a Context instance.
		 *
		 * This class is the builder class to build a Context instance. Since the
		 * building process of Context consists of several stages, and you may want to
		 * create additional resources between these stages (like a window surface) or
		 * only execute a subset of these stages (like build a Vulkan instance and list
		 * all available vulkan physical devices for users to choose from), the building
		 * process is divided into several functions. You need to call them in order.
		 ***********************************************************************/
		class ContextBuilder {

		public:

			/** @brief	Default constructor.
			  */
			ContextBuilder(void) = default;

			/*============================================================
			 *                  Instance Building Stage
			 *============================================================*/

			/** @name	Instance building stage.
			  */
			///@{

			/** @brief	Set headless mode.
			  * @param	headless_	Whether to set headless mode.
			  * 
			  *	By default, headless mode is `false`. The builder will automatically
			  * enable the `VK_KHR_surface` instance extension and `VK_KHR_swapchain`
			  * device extension. However, you still need to manually enable the platform
			  * specific surface extension (e.g. `VK_KHR_win32_surface` on Windows).
			  * 
			  * @sa		enableInstanceExtension
			  */
			ContextBuilder& headless(bool headless_) {
				this->_headless = headless_;
				return *this;
			}

			/** @brief	Enable validation layer.
			  * @param	enable_		Whether to enable validation layer.
			  */
			ContextBuilder& enableValidation(bool enable_) {
				this->_enableValidationLayer = enable_;
				return *this;
			}

			/** @brief	Set application name.
			  * @param	applicationName_	Application name
			  */
			ContextBuilder& applicationName(const std::string& applicationName_) {
				this->_applicationName = applicationName_;
				return *this;
			}

			/** @brief	Set application version.
			  * @param	variant_	Variant version number.
			  * @param	major_		Major version number.
			  * @param	minor_		Minor version number.
			  * @param	patch_		Patch version number.
			  */
			ContextBuilder& applicationVersion(std::uint32_t variant_, std::uint32_t major_, std::uint32_t minor_, std::uint32_t patch_) {
				this->_applicationVersion = VK_MAKE_API_VERSION(variant_, major_, minor_, patch_);
				return *this;
			}

			/** @brief	Set application version.
			  * @param	applicationVersion_		Application version.
			  */
			ContextBuilder& applicationVersion(std::uint32_t applicationVersion_) {
				this->_applicationVersion = applicationVersion_;
				return *this;
			}

			/** @brief	Set engine name.
			  * @param	engineName_		Engine name.
			  */
			ContextBuilder& engineName(const std::string& engineName_) {
				this->_engineName = engineName_;
				return *this;
			}

			/** @brief	Set engine version.
			  * @param	variant_	Variant version number.
			  * @param	major_		Major version number.
			  * @param	minor_		Minor version number.
			  * @param	patch_		Patch version number.
			  */
			ContextBuilder& engineVersion(std::uint32_t variant_, std::uint32_t major_, std::uint32_t minor_, std::uint32_t patch_) {
				this->_engineVersion = VK_MAKE_API_VERSION(variant_, major_, minor_, patch_);
				return *this;
			}

			/** @brief	Set engine version.
			  * @param	engineVersion_		Engine version.
			  */
			ContextBuilder& engineVersion(std::uint32_t engineVersion_) {
				this->_engineVersion = engineVersion_;
				return *this;
			}

			/** @brief	Set API version.
			  * @param	variant_	Variant version number.
			  * @param	major_		Major version number.
			  * @param	minor_		Minor version number.
			  * @param	patch_		Patch version number.
			  */
			ContextBuilder& apiVersion(std::uint32_t variant_, std::uint32_t major_, std::uint32_t minor_, std::uint32_t patch_) {
				this->_apiVersion = VK_MAKE_API_VERSION(variant_, major_, minor_, patch_);
				return *this;
			}

			/** @brief	Set API version.
			  * @param	apiVersion_		API version.
			  */
			ContextBuilder& apiVersion(std::uint32_t apiVersion_) {
				this->_apiVersion = apiVersion_;
				return *this;
			}

			/** @brief	Enable a layer.
			  * @note	If you have already called ContextBuilder::enableValidationLayer to enable
			  *			the validation layer, "VK_LAYER_KHRONOS_validation" layer will be
			  *			automatically enabled. You don't need to call this function to enable it again.
			  * @param	layerName_	The name of the layer.
			  */
			ContextBuilder& enableLayer(const std::string& layerName_) {
				this->_enableLayers.insert(layerName_);
				return *this;
			}

			/** @brief	Enable layers.
			  * @note	If you have already called ContextBuilder::enableValidationLayer to enable
			  *			the validation layer, "VK_LAYER_KHRONOS_validation" layer will be
			  *			automatically enabled. You don't need to call this function to enable it again.
			  * @param	begin_	The begin iterator of a container of layers.
			  * @param	end_	The end iterator of a container of layers.
			  */
			template <class Iterator>
			ContextBuilder& enableLayer(Iterator begin_, Iterator end_) {
				this->_enableLayers.insert(begin_, end_);
				return *this;
			}

			/** @brief	Enable an instance extension.
			  * @note	If you have already called ContextBuilder::setDebugUtilsMessenger to set
			  *			the debug messenger, "VK_EXT_debug_utils" extension will be
			  *			automatically enabled. You don't need to call this function to enable it again.
			  * @param	extensionName_	The name of the extension.
			  */
			ContextBuilder& enableInstanceExtension(const std::string& extensionName_) {
				this->_enableInstanceExtensions.insert(extensionName_);
				return *this;
			}

			/** @brief	Enable instance extensions.
			  * @note	If you have already called ContextBuilder::setDebugUtilsMessenger to set
			  *			the debug messenger, "VK_EXT_debug_utils" extension will be
			  *			automatically added. You don't need to call this function to enable it again.
			  * @param	begin_	The begin iterator of a container of instance extensions.
			  * @param	end_	The end iterator of a container of instance extensions.
			  */
			template <class Iterator>
			ContextBuilder& enableInstanceExtensions(Iterator begin_, Iterator end_) {
				this->_enableInstanceExtensions.insert(begin_, end_);
				return *this;
			}

			/** @brief	Set debug messenger. This will enable the validation layer.
			  * @param	messageSeverity_		Message severity.
			  * @param	messageType_			Message type.
			  * @param	pfnUserCallback_		Callback function.
			  * @param	pUserData_				User data to send to the callback function.
			  */
			ContextBuilder& setDebugUtilsMessenger(
				::vk::DebugUtilsMessageSeverityFlagsEXT messageSeverity_,
				::vk::DebugUtilsMessageTypeFlagsEXT messageType_,
				PFN_vkDebugUtilsMessengerCallbackEXT pfnUserCallback_,
				void* pUserData_ = nullptr
			) {
				this->_enableValidationLayer = true;
				this->_enableDebugUtilsMessenger = true;
				this->_debugUtilsMessengerInfo.messageSeverity = messageSeverity_;
				this->_debugUtilsMessengerInfo.messageType = messageType_;
				this->_debugUtilsMessengerInfo.pfnUserCallback = pfnUserCallback_;
				this->_debugUtilsMessengerInfo.pUserData = pUserData_;
				return *this;
			}

			/** @brief	Use default debug messenger.
			  * 
			  * Call this function if you want to use the default debug callback. The default
			  * debug callback receives warning and error messages.
			  */
			ContextBuilder& useDefaultDebugUtilsMessenger(void) {
				this->setDebugUtilsMessenger(
					::vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
					::vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | ::vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | ::vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
					ContextBuilder::defaultDebugCallback,
					nullptr
				);
				return *this;
			}

			/** @brief	Default debug messenger.
			  */
			static VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData
			);

			///@}

			/*============================================================
			 *             Physical Device Selection Stage
			 *============================================================*/

			/** @name	Physical device selection stage.
			  */
			///@{
			
			/** @brief	Enable a device extension.
			  * @note	If the headless mode is not turned on, "VK_KHR_swapchain" extension
			  *			will be automatically enabled. You don't need to call this function
			  *			to enable it again.
			  * @param	extensionName_	The name of the extension.
			  */
			ContextBuilder& enableDeviceExtension(const std::string& extensionName_) {
				this->_enableDeviceExtensions.insert(extensionName_);
				return *this;
			}

			/** @brief	Enable device extensions.
			  * @note	If the headless mode is not turned on, "VK_KHR_swapchain" extension
			  *			will be automatically enabled. You don't need to call this function
			  *			to enable it again.
			  * @param	begin_	The begin iterator of a container of device extensions.
			  * @param	end_	The end iterator of a container of device extensions.
			  */
			template <class Iterator>
			ContextBuilder& enableDeviceExtensions(Iterator begin_, Iterator end_) {
				this->_enableDeviceExtensions.insert(begin_, end_);
				return *this;
			}

			/** @brief	Set requested physical device type.
			  * @param	type_	The type of the physical device. If it is `std::nullopt`,
			  *					this check will be disabled.
			  * 
			  * By default, the requested physical device type is discrete GPU. The selector
			  * will select a discrete GPU if available.
			  */
			ContextBuilder& requestPhysicalDeviceType(std::optional<::vk::PhysicalDeviceType> type_) {
				this->_requestPhysicalDeviceType = type_;
				return *this;
			}

			/** @brief	Set required physical device type.
			  * @param	type_	The type of the physical device. If it is `std::nullopt`,
			  *					this check will be disabled.
			  *
			  * By default, the required physical device type is discrete GPU. The selector
			  * will select a discrete GPU if available.
			  */
			ContextBuilder& requirePhysicalDeviceType(std::optional<::vk::PhysicalDeviceType> type_) {
				this->_requirePhysicalDeviceType = type_;
				return *this;
			}

			/** @brief	Set requested physical device features to be enabled.
			  * @param	features_	The features to be enabled.
			  */
			ContextBuilder& requestPhysicalDeviceFeatures(const ::vk::PhysicalDeviceFeatures& features_) {
				this->_requestPhysicalDeviceFeatures = features_;
				return *this;
			}

			/** @brief	Set required physical device features to be enabled.
			  * @param	features_	The features to be enabled.
			  */
			ContextBuilder& requirePhysicalDeviceFeatures(const ::vk::PhysicalDeviceFeatures& features_) {
				this->_requirePhysicalDeviceFeatures = features_;
				return *this;
			}

			/** @brief	Add a custom physical device selection criteria.
			  * @param	criteria_	The selection criteria. It is a function that takes a PhysicalDeviceInfo struct, and
			  *						returns a boolean indicating whether this physical device meets the criteria.
			  */
			ContextBuilder& addPhysicalDeviceSelectionCriteria(std::function<bool(const PhysicalDeviceInfo&)> criteria_) {
				this->_physicalDeviceSelectionCriteria.push_back(std::move(criteria_));
				return *this;
			}

			///@}

			/*============================================================
			 *                      Building Stage
			 *============================================================*/

			/** @name	Building stage.
			  */
			///@{

			/** @brief	Build `vk::raii::Instance`.
			  */
			void buildInstance(Context& context_) const;

			/** @brief	List all `vk::raii::PhysicalDevice`s.
			  */
			std::vector<PhysicalDeviceInfo> listPhysicalDevices(const Context& context_) const;

			/** @brief	Select a `vk::raii::PhysicalDevice` that meet the criteria for the context.
			  */
			void selectPhysicalDevice(Context& context_) const;

			/** @brief	Select the given `vk::raii::PhysicalDevice` for the context.
			  */
			void selectPhysicalDevice(Context& context_, const ::vk::raii::PhysicalDevice& physicalDevice_) const;

			///@}

		private:

			// Instance building
			bool _headless = false;
			bool _enableValidationLayer = false;
			std::string _applicationName = "";
			std::uint32_t _applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			std::string _engineName = "";
			std::uint32_t _engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			std::uint32_t _apiVersion = VK_API_VERSION_1_0;
			std::set<std::string> _enableLayers = {};
			std::set<std::string> _enableInstanceExtensions = {};
			bool _enableDebugUtilsMessenger = false;
			struct {
				::vk::DebugUtilsMessageSeverityFlagsEXT messageSeverity;
				::vk::DebugUtilsMessageTypeFlagsEXT messageType;
				PFN_vkDebugUtilsMessengerCallbackEXT pfnUserCallback;
				void* pUserData;
			} _debugUtilsMessengerInfo = {};

			// Physical device selection
			std::set<std::string> _enableDeviceExtensions = {};
			std::optional<::vk::PhysicalDeviceType> _requestPhysicalDeviceType = ::vk::PhysicalDeviceType::eDiscreteGpu;
			std::optional<::vk::PhysicalDeviceType> _requirePhysicalDeviceType = std::nullopt;
			::vk::PhysicalDeviceFeatures _requestPhysicalDeviceFeatures = {};
			::vk::PhysicalDeviceFeatures _requirePhysicalDeviceFeatures = {};
			std::vector<std::function<bool(const PhysicalDeviceInfo&)>> _physicalDeviceSelectionCriteria = {};
			PhysicalDeviceInfo _checkPhysicalDevice(const ::vk::raii::PhysicalDevice& physicalDevice_) const;

		};

	}

}

/*======================================================================
 | Implementation
 ======================================================================*/
/// @cond

namespace jjyou {

	namespace vk {

		inline PhysicalDeviceInfo::Support operator|(PhysicalDeviceInfo::Support x, PhysicalDeviceInfo::Support y) {
			return static_cast<PhysicalDeviceInfo::Support>(static_cast<std::underlying_type_t<PhysicalDeviceInfo::Support>>(x) | static_cast<std::underlying_type_t<PhysicalDeviceInfo::Support>>(y));
		}
		inline PhysicalDeviceInfo::Support& operator|=(PhysicalDeviceInfo::Support& x, PhysicalDeviceInfo::Support y) {
			x = x | y;
			return x;
		}

		inline void ContextBuilder::buildInstance(Context& context_) const {
			context_._headless = this->_headless;
			context_._validation = this->_enableValidationLayer;
			std::set<std::string> enableLayerSet = this->_enableLayers;
			if (this->_enableValidationLayer) {
				enableLayerSet.insert("VK_LAYER_KHRONOS_validation");
			}
			std::vector<const char*> enableLayers;
			enableLayers.reserve(enableLayerSet.size());
			std::transform(
				enableLayerSet.cbegin(), enableLayerSet.cend(),
				std::back_inserter(enableLayers),
				[](const std::string& s) { return s.c_str(); }
			);
			std::set<std::string> enableInstanceExtensionSet = this->_enableInstanceExtensions;
			if (!this->_headless) {
				enableInstanceExtensionSet.insert(VK_KHR_SURFACE_EXTENSION_NAME);
			}
			if (this->_enableDebugUtilsMessenger) {
				enableInstanceExtensionSet.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			}
			std::vector<const char*> enableInstanceExtensions;
			enableInstanceExtensions.reserve(enableInstanceExtensionSet.size());
			std::transform(
				enableInstanceExtensionSet.cbegin(), enableInstanceExtensionSet.cend(),
				std::back_inserter(enableInstanceExtensions),
				[](const std::string& s) { return s.c_str(); }
			);
			::vk::ApplicationInfo applicationInfo(
				this->_applicationName.c_str(),
				this->_applicationVersion,
				this->_engineName.c_str(),
				this->_engineVersion,
				this->_apiVersion
			);
			::vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo(
				::vk::DebugUtilsMessengerCreateFlagsEXT(0U),
				this->_debugUtilsMessengerInfo.messageSeverity,
				this->_debugUtilsMessengerInfo.messageType,
				this->_debugUtilsMessengerInfo.pfnUserCallback,
				this->_debugUtilsMessengerInfo.pUserData
			);
			::vk::InstanceCreateInfo instanceCreateInfo(
				::vk::InstanceCreateFlags(0U),
				&applicationInfo,
				enableLayers,
				enableInstanceExtensions
			);
			if (this->_enableDebugUtilsMessenger)
				instanceCreateInfo.setPNext(&debugUtilsMessengerCreateInfo);
			context_._instance = ::vk::raii::Instance(context_._context, instanceCreateInfo);
			context_._enabledLayers = std::move(enableLayerSet);
			context_._enabledInstanceExtensions = std::move(enableInstanceExtensionSet);
			if (this->_enableDebugUtilsMessenger) {
				::vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo(
					::vk::DebugUtilsMessengerCreateFlagsEXT(0U),
					this->_debugUtilsMessengerInfo.messageSeverity,
					this->_debugUtilsMessengerInfo.messageType,
					this->_debugUtilsMessengerInfo.pfnUserCallback,
					this->_debugUtilsMessengerInfo.pUserData
				);
				context_._debugUtilsMessenger = ::vk::raii::DebugUtilsMessengerEXT(context_._instance, debugUtilsMessengerCreateInfo);
			}
		}

		inline std::vector<PhysicalDeviceInfo> ContextBuilder::listPhysicalDevices(const Context& context_) const {
			std::vector<PhysicalDeviceInfo> res;
			std::vector<::vk::raii::PhysicalDevice> physicalDevices = context_._instance.enumeratePhysicalDevices();
			res.reserve(physicalDevices.size());
			std::transform(
				physicalDevices.cbegin(), physicalDevices.cend(),
				std::back_inserter(res),
				[&](const ::vk::raii::PhysicalDevice& physicalDevice_) {return this->_checkPhysicalDevice(physicalDevice_); }
			);
			return res;
		}

		inline void ContextBuilder::selectPhysicalDevice(Context& context_) const {
			std::vector<PhysicalDeviceInfo> physicalDeviceInfos = this->listPhysicalDevices(context_);
			std::optional<std::size_t> candidate = std::nullopt;
			for (std::size_t i = 0; i < physicalDeviceInfos.size(); ++i) {
				const PhysicalDeviceInfo& physicalDeviceInfo = physicalDeviceInfos[i];
				if (physicalDeviceInfo.requiredCriteria == PhysicalDeviceInfo::Support::AllSupported) {
					if (physicalDeviceInfo.requestedCriteria == PhysicalDeviceInfo::Support::AllSupported) {
						candidate = i;
						break;
					}
					else if (!candidate.has_value()) {
						candidate = i;
					}
				}
			}
			if (!candidate.has_value()) {
				::vk::resultCheck(::vk::Result::eErrorInitializationFailed, "[Vulkan ContextBuilder] No physical device meets the selection criteria.");
				return;
			}
			const PhysicalDeviceInfo& physicalDeviceInfo = physicalDeviceInfos[*candidate];
			context_._physicalDevice = physicalDeviceInfo.physicalDevice;
			context_._enabledDeviceFeatures = physicalDeviceInfo.enabledDeviceFeatures;
			context_._enabledDeviceExtensions = physicalDeviceInfo.enabledDeviceExtensions;
			context_._queueFamilyIndices = physicalDeviceInfo.queueFamilyIndices;
		}

		inline void ContextBuilder::selectPhysicalDevice(Context& context_, const ::vk::raii::PhysicalDevice& physicalDevice_) const {
			const PhysicalDeviceInfo physicalDeviceInfo = this->_checkPhysicalDevice(physicalDevice_);
			if (physicalDeviceInfo.requiredCriteria != PhysicalDeviceInfo::Support::AllSupported) {
				::vk::resultCheck(::vk::Result::eErrorInitializationFailed, "[Vulkan ContextBuilder] The given physical device does not meet the selection criteria.");
				return;
			}
			context_._physicalDevice = physicalDeviceInfo.physicalDevice;
			context_._enabledDeviceFeatures = physicalDeviceInfo.enabledDeviceFeatures;
			context_._enabledDeviceExtensions = physicalDeviceInfo.enabledDeviceExtensions;
			context_._queueFamilyIndices = physicalDeviceInfo.queueFamilyIndices;
		}

		inline VKAPI_ATTR VkBool32 VKAPI_CALL ContextBuilder::defaultDebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		) {
			auto msgSeverity = [](VkDebugUtilsMessageSeverityFlagBitsEXT s) -> std::string {
				std::string ret;
				if (s & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
					ret += "VERBOSE | ";
				if (s & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
					ret += "INFO | ";
				if (s & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
					ret += "WARNING | ";
				if (s & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
					ret += "ERROR | ";
				if (ret.length())
					return ret.substr(0, ret.length() - 2);
				return "NONE";
				};
			auto msgType = [](VkDebugUtilsMessageTypeFlagsEXT s) -> std::string {
				std::string ret;
				if (s & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
					ret += "GENERAL | ";
				if (s & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
					ret += "VALIDATION | ";
				if (s & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
					ret += "PERFORMANCE | ";
				if (ret.length())
					return ret.substr(0, ret.length() - 2);
				return "NONE";
				};
			std::cerr << std::format("[Vulkan Debug Callback] severity: {}, type: {}, message: {}", msgSeverity(messageSeverity), msgType(messageType), pCallbackData->pMessage) << std::endl;
			return VK_FALSE;
		}

		inline PhysicalDeviceInfo ContextBuilder::_checkPhysicalDevice(const ::vk::raii::PhysicalDevice& physicalDevice_) const {
			PhysicalDeviceInfo res;
			res.physicalDevice = physicalDevice_;
			res.requestedCriteria = PhysicalDeviceInfo::Support::AllSupported;
			res.requiredCriteria = PhysicalDeviceInfo::Support::AllSupported;
			// Check physical device type
			::vk::PhysicalDeviceProperties properties = physicalDevice_.getProperties();
			res.type = properties.deviceType;
			if (this->_requestPhysicalDeviceType.has_value() && this->_requestPhysicalDeviceType != res.type)
				res.requestedCriteria |= PhysicalDeviceInfo::Support::TypeNotMatched;
			if (this->_requirePhysicalDeviceType.has_value() && this->_requirePhysicalDeviceType != res.type)
				res.requiredCriteria |= PhysicalDeviceInfo::Support::TypeNotMatched;
			// Check physical device feature
			::vk::PhysicalDeviceFeatures features = physicalDevice_.getFeatures();
			for (std::size_t i = 0; i < sizeof(::vk::PhysicalDeviceFeatures) / sizeof(::vk::Bool32); ++i) {
				::vk::Bool32 requested = reinterpret_cast<const ::vk::Bool32*>(&this->_requestPhysicalDeviceFeatures)[i];
				::vk::Bool32 required = reinterpret_cast<const ::vk::Bool32*>(&this->_requirePhysicalDeviceFeatures)[i];
				::vk::Bool32 feature = reinterpret_cast<const ::vk::Bool32*>(&features)[i];
				::vk::Bool32& enable = reinterpret_cast<::vk::Bool32*>(&res.enabledDeviceFeatures)[i];
				if (requested && !feature)
					res.requestedCriteria |= PhysicalDeviceInfo::Support::FeatureNotSupported;
				if (required && !feature)
					res.requiredCriteria |= PhysicalDeviceInfo::Support::FeatureNotSupported;
				enable = ((requested || required) && feature) ? (::vk::True) : (::vk::False);
			}
			// Check extension support
			std::set<std::string> enableDeviceExtensionSet = this->_enableDeviceExtensions;
			if (!this->_headless) {
				enableDeviceExtensionSet.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			}
			std::vector<::vk::ExtensionProperties> availableDeviceExtensions = physicalDevice_.enumerateDeviceExtensionProperties();
			for (const auto& requiredDeviceExtension : enableDeviceExtensionSet) {
				for (const auto& availableDeviceExtension : availableDeviceExtensions)
					if (availableDeviceExtension.extensionName == requiredDeviceExtension) {
						res.enabledDeviceExtensions.insert(requiredDeviceExtension);
						break;
					}
			}
			if (res.enabledDeviceExtensions.size() != enableDeviceExtensionSet.size())
				res.requiredCriteria |= PhysicalDeviceInfo::Support::ExtensionNotSupported;
			// Get queue family indices
			std::vector<::vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice_.getQueueFamilyProperties();
			for (std::size_t i = 0; i < queueFamilyProperties.size(); ++i) {
				const auto& queueFamilyProperty = queueFamilyProperties[i];
				if (!res.queueFamilyIndices[Context::QueueType::Main].has_value() &&
					(queueFamilyProperty.queueFlags & ::vk::QueueFlagBits::eGraphics) &&
					(queueFamilyProperty.queueFlags & ::vk::QueueFlagBits::eCompute) &&
					(queueFamilyProperty.queueFlags & ::vk::QueueFlagBits::eTransfer))
					res.queueFamilyIndices[Context::QueueType::Main] = static_cast<std::uint32_t>(i);
				if (!res.queueFamilyIndices[Context::QueueType::Compute].has_value() &&
					(queueFamilyProperty.queueFlags & ::vk::QueueFlagBits::eCompute) &&
					(queueFamilyProperty.queueFlags & ::vk::QueueFlagBits::eTransfer) &&
					(!res.queueFamilyIndices[Context::QueueType::Main].has_value() || *res.queueFamilyIndices[Context::QueueType::Main] != i))
					res.queueFamilyIndices[Context::QueueType::Compute] = static_cast<std::uint32_t>(i);
				if (!res.queueFamilyIndices[Context::QueueType::Transfer].has_value() &&
					(queueFamilyProperty.queueFlags & ::vk::QueueFlagBits::eTransfer) &&
					(!res.queueFamilyIndices[Context::QueueType::Main].has_value() || *res.queueFamilyIndices[Context::QueueType::Main] != i) &&
					(!res.queueFamilyIndices[Context::QueueType::Compute].has_value() || *res.queueFamilyIndices[Context::QueueType::Compute] != i))
					res.queueFamilyIndices[Context::QueueType::Transfer] = static_cast<std::uint32_t>(i);
				if (res.queueFamilyIndices[Context::QueueType::Main].has_value() &&
					res.queueFamilyIndices[Context::QueueType::Compute].has_value() &&
					res.queueFamilyIndices[Context::QueueType::Transfer].has_value())
					break;
			}
			if (!res.queueFamilyIndices[Context::QueueType::Main].has_value())
				res.requiredCriteria |= PhysicalDeviceInfo::Support::QueueNotFound;
			if (!res.queueFamilyIndices[Context::QueueType::Compute].has_value() || !res.queueFamilyIndices[Context::QueueType::Transfer].has_value())
				res.requestedCriteria |= PhysicalDeviceInfo::Support::QueueNotFound;
			// Check user defined criteria
			for (const auto& criteria : this->_physicalDeviceSelectionCriteria)
				if (!criteria(res)) {
					res.requiredCriteria |= PhysicalDeviceInfo::Support::UserDefinedCriteriaNotMet;
					break;
				}
			// Done
			return res;
		}

	}

}

/// @endcond

#endif /* jjyou_vk_Context_hpp */