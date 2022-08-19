#pragma once

#include "../core/types.hpp"
#include <compare>

namespace VGED {
    namespace Engine {
        enum class Format {
            UNDEFINED = 0,
            R4G4_UNORM_PACK8 = 1,
            R4G4B4A4_UNORM_PACK16 = 2,
            B4G4R4A4_UNORM_PACK16 = 3,
            R5G6B5_UNORM_PACK16 = 4,
            B5G6R5_UNORM_PACK16 = 5,
            R5G5B5A1_UNORM_PACK16 = 6,
            B5G5R5A1_UNORM_PACK16 = 7,
            A1R5G5B5_UNORM_PACK16 = 8,
            R8_UNORM = 9,
            R8_SNORM = 10,
            R8_USCALED = 11,
            R8_SSCALED = 12,
            R8_UINT = 13,
            R8_SINT = 14,
            R8_SRGB = 15,
            R8G8_UNORM = 16,
            R8G8_SNORM = 17,
            R8G8_USCALED = 18,
            R8G8_SSCALED = 19,
            R8G8_UINT = 20,
            R8G8_SINT = 21,
            R8G8_SRGB = 22,
            R8G8B8_UNORM = 23,
            R8G8B8_SNORM = 24,
            R8G8B8_USCALED = 25,
            R8G8B8_SSCALED = 26,
            R8G8B8_UINT = 27,
            R8G8B8_SINT = 28,
            R8G8B8_SRGB = 29,
            B8G8R8_UNORM = 30,
            B8G8R8_SNORM = 31,
            B8G8R8_USCALED = 32,
            B8G8R8_SSCALED = 33,
            B8G8R8_UINT = 34,
            B8G8R8_SINT = 35,
            B8G8R8_SRGB = 36,
            R8G8B8A8_UNORM = 37,
            R8G8B8A8_SNORM = 38,
            R8G8B8A8_USCALED = 39,
            R8G8B8A8_SSCALED = 40,
            R8G8B8A8_UINT = 41,
            R8G8B8A8_SINT = 42,
            R8G8B8A8_SRGB = 43,
            B8G8R8A8_UNORM = 44,
            B8G8R8A8_SNORM = 45,
            B8G8R8A8_USCALED = 46,
            B8G8R8A8_SSCALED = 47,
            B8G8R8A8_UINT = 48,
            B8G8R8A8_SINT = 49,
            B8G8R8A8_SRGB = 50,
            A8B8G8R8_UNORM_PACK32 = 51,
            A8B8G8R8_SNORM_PACK32 = 52,
            A8B8G8R8_USCALED_PACK32 = 53,
            A8B8G8R8_SSCALED_PACK32 = 54,
            A8B8G8R8_UINT_PACK32 = 55,
            A8B8G8R8_SINT_PACK32 = 56,
            A8B8G8R8_SRGB_PACK32 = 57,
            A2R10G10B10_UNORM_PACK32 = 58,
            A2R10G10B10_SNORM_PACK32 = 59,
            A2R10G10B10_USCALED_PACK32 = 60,
            A2R10G10B10_SSCALED_PACK32 = 61,
            A2R10G10B10_UINT_PACK32 = 62,
            A2R10G10B10_SINT_PACK32 = 63,
            A2B10G10R10_UNORM_PACK32 = 64,
            A2B10G10R10_SNORM_PACK32 = 65,
            A2B10G10R10_USCALED_PACK32 = 66,
            A2B10G10R10_SSCALED_PACK32 = 67,
            A2B10G10R10_UINT_PACK32 = 68,
            A2B10G10R10_SINT_PACK32 = 69,
            R16_UNORM = 70,
            R16_SNORM = 71,
            R16_USCALED = 72,
            R16_SSCALED = 73,
            R16_UINT = 74,
            R16_SINT = 75,
            R16_SFLOAT = 76,
            R16G16_UNORM = 77,
            R16G16_SNORM = 78,
            R16G16_USCALED = 79,
            R16G16_SSCALED = 80,
            R16G16_UINT = 81,
            R16G16_SINT = 82,
            R16G16_SFLOAT = 83,
            R16G16B16_UNORM = 84,
            R16G16B16_SNORM = 85,
            R16G16B16_USCALED = 86,
            R16G16B16_SSCALED = 87,
            R16G16B16_UINT = 88,
            R16G16B16_SINT = 89,
            R16G16B16_SFLOAT = 90,
            R16G16B16A16_UNORM = 91,
            R16G16B16A16_SNORM = 92,
            R16G16B16A16_USCALED = 93,
            R16G16B16A16_SSCALED = 94,
            R16G16B16A16_UINT = 95,
            R16G16B16A16_SINT = 96,
            R16G16B16A16_SFLOAT = 97,
            R32_UINT = 98,
            R32_SINT = 99,
            R32_SFLOAT = 100,
            R32G32_UINT = 101,
            R32G32_SINT = 102,
            R32G32_SFLOAT = 103,
            R32G32B32_UINT = 104,
            R32G32B32_SINT = 105,
            R32G32B32_SFLOAT = 106,
            R32G32B32A32_UINT = 107,
            R32G32B32A32_SINT = 108,
            R32G32B32A32_SFLOAT = 109,
            R64_UINT = 110,
            R64_SINT = 111,
            R64_SFLOAT = 112,
            R64G64_UINT = 113,
            R64G64_SINT = 114,
            R64G64_SFLOAT = 115,
            R64G64B64_UINT = 116,
            R64G64B64_SINT = 117,
            R64G64B64_SFLOAT = 118,
            R64G64B64A64_UINT = 119,
            R64G64B64A64_SINT = 120,
            R64G64B64A64_SFLOAT = 121,
            B10G11R11_UFLOAT_PACK32 = 122,
            E5B9G9R9_UFLOAT_PACK32 = 123,
            D16_UNORM = 124,
            X8_D24_UNORM_PACK32 = 125,
            D32_SFLOAT = 126,
            S8_UINT = 127,
            D16_UNORM_S8_UINT = 128,
            D24_UNORM_S8_UINT = 129,
            D32_SFLOAT_S8_UINT = 130,
            BC1_RGB_UNORM_BLOCK = 131,
            BC1_RGB_SRGB_BLOCK = 132,
            BC1_RGBA_UNORM_BLOCK = 133,
            BC1_RGBA_SRGB_BLOCK = 134,
            BC2_UNORM_BLOCK = 135,
            BC2_SRGB_BLOCK = 136,
            BC3_UNORM_BLOCK = 137,
            BC3_SRGB_BLOCK = 138,
            BC4_UNORM_BLOCK = 139,
            BC4_SNORM_BLOCK = 140,
            BC5_UNORM_BLOCK = 141,
            BC5_SNORM_BLOCK = 142,
            BC6H_UFLOAT_BLOCK = 143,
            BC6H_SFLOAT_BLOCK = 144,
            BC7_UNORM_BLOCK = 145,
            BC7_SRGB_BLOCK = 146,
            ETC2_R8G8B8_UNORM_BLOCK = 147,
            ETC2_R8G8B8_SRGB_BLOCK = 148,
            ETC2_R8G8B8A1_UNORM_BLOCK = 149,
            ETC2_R8G8B8A1_SRGB_BLOCK = 150,
            ETC2_R8G8B8A8_UNORM_BLOCK = 151,
            ETC2_R8G8B8A8_SRGB_BLOCK = 152,
            EAC_R11_UNORM_BLOCK = 153,
            EAC_R11_SNORM_BLOCK = 154,
            EAC_R11G11_UNORM_BLOCK = 155,
            EAC_R11G11_SNORM_BLOCK = 156,
            ASTC_4x4_UNORM_BLOCK = 157,
            ASTC_4x4_SRGB_BLOCK = 158,
            ASTC_5x4_UNORM_BLOCK = 159,
            ASTC_5x4_SRGB_BLOCK = 160,
            ASTC_5x5_UNORM_BLOCK = 161,
            ASTC_5x5_SRGB_BLOCK = 162,
            ASTC_6x5_UNORM_BLOCK = 163,
            ASTC_6x5_SRGB_BLOCK = 164,
            ASTC_6x6_UNORM_BLOCK = 165,
            ASTC_6x6_SRGB_BLOCK = 166,
            ASTC_8x5_UNORM_BLOCK = 167,
            ASTC_8x5_SRGB_BLOCK = 168,
            ASTC_8x6_UNORM_BLOCK = 169,
            ASTC_8x6_SRGB_BLOCK = 170,
            ASTC_8x8_UNORM_BLOCK = 171,
            ASTC_8x8_SRGB_BLOCK = 172,
            ASTC_10x5_UNORM_BLOCK = 173,
            ASTC_10x5_SRGB_BLOCK = 174,
            ASTC_10x6_UNORM_BLOCK = 175,
            ASTC_10x6_SRGB_BLOCK = 176,
            ASTC_10x8_UNORM_BLOCK = 177,
            ASTC_10x8_SRGB_BLOCK = 178,
            ASTC_10x10_UNORM_BLOCK = 179,
            ASTC_10x10_SRGB_BLOCK = 180,
            ASTC_12x10_UNORM_BLOCK = 181,
            ASTC_12x10_SRGB_BLOCK = 182,
            ASTC_12x12_UNORM_BLOCK = 183,
            ASTC_12x12_SRGB_BLOCK = 184,
            G8B8G8R8_422_UNORM = 1000156000,
            B8G8R8G8_422_UNORM = 1000156001,
            G8_B8_R8_3PLANE_420_UNORM = 1000156002,
            G8_B8R8_2PLANE_420_UNORM = 1000156003,
            G8_B8_R8_3PLANE_422_UNORM = 1000156004,
            G8_B8R8_2PLANE_422_UNORM = 1000156005,
            G8_B8_R8_3PLANE_444_UNORM = 1000156006,
            R10X6_UNORM_PACK16 = 1000156007,
            R10X6G10X6_UNORM_2PACK16 = 1000156008,
            R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 1000156009,
            G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 1000156010,
            B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 1000156011,
            G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 1000156012,
            G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 1000156013,
            G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 1000156014,
            G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 1000156015,
            G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 1000156016,
            R12X4_UNORM_PACK16 = 1000156017,
            R12X4G12X4_UNORM_2PACK16 = 1000156018,
            R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 1000156019,
            G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 1000156020,
            B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 1000156021,
            G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 1000156022,
            G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 1000156023,
            G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 1000156024,
            G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 1000156025,
            G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 1000156026,
            G16B16G16R16_422_UNORM = 1000156027,
            B16G16R16G16_422_UNORM = 1000156028,
            G16_B16_R16_3PLANE_420_UNORM = 1000156029,
            G16_B16R16_2PLANE_420_UNORM = 1000156030,
            G16_B16_R16_3PLANE_422_UNORM = 1000156031,
            G16_B16R16_2PLANE_422_UNORM = 1000156032,
            G16_B16_R16_3PLANE_444_UNORM = 1000156033,
            G8_B8R8_2PLANE_444_UNORM = 1000330000,
            G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16 = 1000330001,
            G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16 = 1000330002,
            G16_B16R16_2PLANE_444_UNORM = 1000330003,
            A4R4G4B4_UNORM_PACK16 = 1000340000,
            A4B4G4R4_UNORM_PACK16 = 1000340001,
            ASTC_4x4_SFLOAT_BLOCK = 1000066000,
            ASTC_5x4_SFLOAT_BLOCK = 1000066001,
            ASTC_5x5_SFLOAT_BLOCK = 1000066002,
            ASTC_6x5_SFLOAT_BLOCK = 1000066003,
            ASTC_6x6_SFLOAT_BLOCK = 1000066004,
            ASTC_8x5_SFLOAT_BLOCK = 1000066005,
            ASTC_8x6_SFLOAT_BLOCK = 1000066006,
            ASTC_8x8_SFLOAT_BLOCK = 1000066007,
            ASTC_10x5_SFLOAT_BLOCK = 1000066008,
            ASTC_10x6_SFLOAT_BLOCK = 1000066009,
            ASTC_10x8_SFLOAT_BLOCK = 1000066010,
            ASTC_10x10_SFLOAT_BLOCK = 1000066011,
            ASTC_12x10_SFLOAT_BLOCK = 1000066012,
            ASTC_12x12_SFLOAT_BLOCK = 1000066013,
            PVRTC1_2BPP_UNORM_BLOCK_IMG = 1000054000,
            PVRTC1_4BPP_UNORM_BLOCK_IMG = 1000054001,
            PVRTC2_2BPP_UNORM_BLOCK_IMG = 1000054002,
            PVRTC2_4BPP_UNORM_BLOCK_IMG = 1000054003,
            PVRTC1_2BPP_SRGB_BLOCK_IMG = 1000054004,
            PVRTC1_4BPP_SRGB_BLOCK_IMG = 1000054005,
            PVRTC2_2BPP_SRGB_BLOCK_IMG = 1000054006,
            PVRTC2_4BPP_SRGB_BLOCK_IMG = 1000054007,
            ASTC_4x4_SFLOAT_BLOCK_EXT = ASTC_4x4_SFLOAT_BLOCK,
            ASTC_5x4_SFLOAT_BLOCK_EXT = ASTC_5x4_SFLOAT_BLOCK,
            ASTC_5x5_SFLOAT_BLOCK_EXT = ASTC_5x5_SFLOAT_BLOCK,
            ASTC_6x5_SFLOAT_BLOCK_EXT = ASTC_6x5_SFLOAT_BLOCK,
            ASTC_6x6_SFLOAT_BLOCK_EXT = ASTC_6x6_SFLOAT_BLOCK,
            ASTC_8x5_SFLOAT_BLOCK_EXT = ASTC_8x5_SFLOAT_BLOCK,
            ASTC_8x6_SFLOAT_BLOCK_EXT = ASTC_8x6_SFLOAT_BLOCK,
            ASTC_8x8_SFLOAT_BLOCK_EXT = ASTC_8x8_SFLOAT_BLOCK,
            ASTC_10x5_SFLOAT_BLOCK_EXT = ASTC_10x5_SFLOAT_BLOCK,
            ASTC_10x6_SFLOAT_BLOCK_EXT = ASTC_10x6_SFLOAT_BLOCK,
            ASTC_10x8_SFLOAT_BLOCK_EXT = ASTC_10x8_SFLOAT_BLOCK,
            ASTC_10x10_SFLOAT_BLOCK_EXT = ASTC_10x10_SFLOAT_BLOCK,
            ASTC_12x10_SFLOAT_BLOCK_EXT = ASTC_12x10_SFLOAT_BLOCK,
            ASTC_12x12_SFLOAT_BLOCK_EXT = ASTC_12x12_SFLOAT_BLOCK,
            G8B8G8R8_422_UNORM_KHR = G8B8G8R8_422_UNORM,
            B8G8R8G8_422_UNORM_KHR = B8G8R8G8_422_UNORM,
            G8_B8_R8_3PLANE_420_UNORM_KHR = G8_B8_R8_3PLANE_420_UNORM,
            G8_B8R8_2PLANE_420_UNORM_KHR = G8_B8R8_2PLANE_420_UNORM,
            G8_B8_R8_3PLANE_422_UNORM_KHR = G8_B8_R8_3PLANE_422_UNORM,
            G8_B8R8_2PLANE_422_UNORM_KHR = G8_B8R8_2PLANE_422_UNORM,
            G8_B8_R8_3PLANE_444_UNORM_KHR = G8_B8_R8_3PLANE_444_UNORM,
            R10X6_UNORM_PACK16_KHR = R10X6_UNORM_PACK16,
            R10X6G10X6_UNORM_2PACK16_KHR = R10X6G10X6_UNORM_2PACK16,
            R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR = R10X6G10X6B10X6A10X6_UNORM_4PACK16,
            G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR = G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
            B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR = B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
            G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR = G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
            G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR = G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
            G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR = G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
            G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR = G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
            G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR = G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
            R12X4_UNORM_PACK16_KHR = R12X4_UNORM_PACK16,
            R12X4G12X4_UNORM_2PACK16_KHR = R12X4G12X4_UNORM_2PACK16,
            R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR = R12X4G12X4B12X4A12X4_UNORM_4PACK16,
            G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR = G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
            B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR = B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
            G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR = G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
            G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR = G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
            G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR = G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
            G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR = G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
            G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR = G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
            G16B16G16R16_422_UNORM_KHR = G16B16G16R16_422_UNORM,
            B16G16R16G16_422_UNORM_KHR = B16G16R16G16_422_UNORM,
            G16_B16_R16_3PLANE_420_UNORM_KHR = G16_B16_R16_3PLANE_420_UNORM,
            G16_B16R16_2PLANE_420_UNORM_KHR = G16_B16R16_2PLANE_420_UNORM,
            G16_B16_R16_3PLANE_422_UNORM_KHR = G16_B16_R16_3PLANE_422_UNORM,
            G16_B16R16_2PLANE_422_UNORM_KHR = G16_B16R16_2PLANE_422_UNORM,
            G16_B16_R16_3PLANE_444_UNORM_KHR = G16_B16_R16_3PLANE_444_UNORM,
            G8_B8R8_2PLANE_444_UNORM_EXT = G8_B8R8_2PLANE_444_UNORM,
            G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT = G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16,
            G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT = G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16,
            G16_B16R16_2PLANE_444_UNORM_EXT = G16_B16R16_2PLANE_444_UNORM,
            A4R4G4B4_UNORM_PACK16_EXT = A4R4G4B4_UNORM_PACK16,
            A4B4G4R4_UNORM_PACK16_EXT = A4B4G4R4_UNORM_PACK16,
        };

        using ImageUsageFlags = u32;
        struct ImageUsageFlagBits {
            static inline constexpr ImageUsageFlags TRANSFER_SRC = 0x00000001;
            static inline constexpr ImageUsageFlags TRANSFER_DST = 0x00000002;
            static inline constexpr ImageUsageFlags SAMPLED = 0x00000004;
            static inline constexpr ImageUsageFlags STORAGE = 0x00000008;
            static inline constexpr ImageUsageFlags COLOR_ATTACHMENT = 0x00000010;
            static inline constexpr ImageUsageFlags DEPTH_STENCIL_ATTACHMENT = 0x00000020;
            static inline constexpr ImageUsageFlags TRANSIENT_ATTACHMENT = 0x00000040;
            static inline constexpr ImageUsageFlags INPUT_ATTACHMENT = 0x00000080;
            static inline constexpr ImageUsageFlags FRAGMENT_DENSITY_MAP = 0x00000200;
            static inline constexpr ImageUsageFlags FRAGMENT_SHADING_RATE_ATTACHMENT = 0x00000100;
            static inline constexpr ImageUsageFlags SHADING_RATE_IMAGE = FRAGMENT_SHADING_RATE_ATTACHMENT;
        };

        enum class ImageLayout {
            UNDEFINED = 0,
            GENERAL = 1,
            COLOR_ATTACHMENT_OPTIMAL = 2,
            DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
            DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
            SHADER_READ_ONLY_OPTIMAL = 5,
            TRANSFER_SRC_OPTIMAL = 6,
            TRANSFER_DST_OPTIMAL = 7,
            PREINITIALIZED = 8,
            DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL = 1000117000,
            DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL = 1000117001,
            DEPTH_ATTACHMENT_OPTIMAL = 1000241000,
            DEPTH_READ_ONLY_OPTIMAL = 1000241001,
            STENCIL_ATTACHMENT_OPTIMAL = 1000241002,
            STENCIL_READ_ONLY_OPTIMAL = 1000241003,
            READ_ONLY_OPTIMAL = 1000314000,
            ATTACHMENT_OPTIMAL = 1000314001,
            PRESENT_SRC = 1000001002,
            SHARED_PRESENT = 1000111000,
            FRAGMENT_DENSITY_MAP_OPTIMAL_EXT = 1000218000,
            FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL = 1000164003,
        };

        using ImageAspectFlags = u32;
        struct ImageAspectFlagBits {
            static inline constexpr ImageAspectFlags NONE = {};
            static inline constexpr ImageAspectFlags COLOR = 0x00000001;
            static inline constexpr ImageAspectFlags DEPTH = 0x00000002;
            static inline constexpr ImageAspectFlags STENCIL = 0x00000004;
            static inline constexpr ImageAspectFlags METADATA = 0x00000008;
            static inline constexpr ImageAspectFlags PLANE_0 = 0x00000010;
            static inline constexpr ImageAspectFlags PLANE_1 = 0x00000020;
            static inline constexpr ImageAspectFlags PLANE_2 = 0x00000040;
            static inline constexpr ImageAspectFlags MEMORY_PLANE_0_EXT = 0x00000080;
            static inline constexpr ImageAspectFlags MEMORY_PLANE_1_EXT = 0x00000100;
            static inline constexpr ImageAspectFlags MEMORY_PLANE_2_EXT = 0x00000200;
            static inline constexpr ImageAspectFlags MEMORY_PLANE_3_EXT = 0x00000400;
            static inline constexpr ImageAspectFlags PLANE_0_KHR = 0x00000080;
            static inline constexpr ImageAspectFlags PLANE_1_KHR = 0x00000100;
            static inline constexpr ImageAspectFlags PLANE_2_KHR = 0x00000200;
            static inline constexpr ImageAspectFlags NONE_KHR  = {};
            static inline constexpr ImageAspectFlags FLAG_BITS_MAX_ENUM = 0x7FFFFFFF;
        };

        using ImageCreateFlags = u32;
        struct ImageCreateFlagBits {
            static inline constexpr ImageCreateFlags NONE = {};
            static inline constexpr ImageCreateFlags SPARSE_BINDING = 0x00000001;
            static inline constexpr ImageCreateFlags SPARSE_RESIDENCY = 0x00000002;
            static inline constexpr ImageCreateFlags SPARSE_ALIASED = 0x00000004;
            static inline constexpr ImageCreateFlags MUTABLE_FORMAT = 0x00000008;
            static inline constexpr ImageCreateFlags CUBE_COMPATIBLE = 0x00000010;
            static inline constexpr ImageCreateFlags ALIAS = 0x00000400;
            static inline constexpr ImageCreateFlags SPLIT_INSTANCE_BIND_REGIONS = 0x00000040;
            static inline constexpr ImageCreateFlags ARRAY_2D_COMPATIBLE = 0x00000020;
            static inline constexpr ImageCreateFlags BLOCK_TEXEL_VIEW_COMPATIBLE = 0x00000080;
            static inline constexpr ImageCreateFlags EXTENDED_USAGE = 0x00000100;
            static inline constexpr ImageCreateFlags PROTECTED = 0x00000800;
            static inline constexpr ImageCreateFlags DISJOINT = 0x00000200;
            static inline constexpr ImageCreateFlags CORNER_SAMPLED_NV = 0x00002000;
            static inline constexpr ImageCreateFlags SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT = 0x00001000;
            static inline constexpr ImageCreateFlags SUBSAMPLED_BIT_EXT = 0x00004000;
            static inline constexpr ImageCreateFlags MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_BIT_EXT = 0x00040000;
            static inline constexpr ImageCreateFlags VIEW_2D_COMPATIBLE_BIT_EXT = 0x00020000;
            static inline constexpr ImageCreateFlags FRAGMENT_DENSITY_MAP_OFFSET_BIT_QCOM = 0x00008000;
            static inline constexpr ImageCreateFlags SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR = 0x00000040;
            static inline constexpr ImageCreateFlags ARRAY_2D_COMPATIBLE_BIT_KHR = 0x00000020;
            static inline constexpr ImageCreateFlags BLOCK_TEXEL_VIEW_COMPATIBLE_BIT_KHR = 0x00000080;
            static inline constexpr ImageCreateFlags EXTENDED_USAGE_BIT_KHR = 0x00000100;
            static inline constexpr ImageCreateFlags DISJOINT_BIT_KHR = 0x00000200;
            static inline constexpr ImageCreateFlags ALIAS_BIT_KHR = 0x00000400;
        };

        enum class ImageType {
            TYPE_1D = 0,
            TYPE_2D = 1,
            TYPE_3D = 2,
            TYPE_MAX_ENUM = 0x7FFFFFFF
        };

        enum class ImageViewType {
            TYPE_1D = 0,
            TYPE_2D = 1,
            TYPE_3D = 2,
            TYPE_CUBE = 3,
            TYPE_1D_ARRAY = 4,
            TYPE_2D_ARRAY = 5,
            TYPE_CUBE_ARRAY = 6,
            TYPE_MAX_ENUM = 0x7FFFFFFF
        };

        enum class Filter {
            NEAREST = 0,
            LINEAR = 1,
            CUBIC_IMG = 1000015000,
            CUBIC_EXT = 1000015000,
            MAX_ENUM = 0x7FFFFFFF
        };

        enum class SamplerAddressMode {
            REPEAT = 0,
            MIRRORED_REPEAT = 1,
            CLAMP_TO_EDGE = 2,
            CLAMP_TO_BORDER = 3,
            MIRROR_CLAMP_TO_EDGE = 4,
            MIRROR_CLAMP_TO_EDGE_KHR = 4,
            MAX_ENUM = 0x7FFFFFFF
        };

        enum class SamplerMipmapMode {
            NEAREST = 0,
            LINEAR = 1,
            MAX_ENUM = 0x7FFFFFFF
        };

        enum class ComponentSwizzle {
            IDENTITY = 0,
            ZERO = 1,
            ONE = 2,
            R = 3,
            G = 4,
            B = 5,
            A = 6,
            MAX_ENUM = 0x7FFFFFFF
        };

        struct ComponentMapping {
            ComponentSwizzle r;
            ComponentSwizzle g;
            ComponentSwizzle b;
            ComponentSwizzle a;

            operator VkComponentMapping const&() const noexcept {
                return *reinterpret_cast<const VkComponentMapping*>(this);
            }
        };

        enum class LoadOp {
            LOAD = 0,
            CLEAR = 1,
            DONT_CARE = 2,
            NONE_EXT = 1000400000,
            MAX_ENUM = 0x7FFFFFFF
        };

        enum class StoreOp {
            STORE = 0,
            DONT_CARE = 1,
            NONE = 1000301000,
            NONE_KHR = 1000301000,
            NONE_QCOM = 1000301000,
            NONE_EXT = 1000301000,
            MAX_ENUM = 0x7FFFFFFF
        };

        using MemoryFlags = u32;
        struct MemoryFlagBits {
            static inline constexpr MemoryFlags DEDICATED_MEMORY = 0x00000001;
            static inline constexpr MemoryFlags CAN_ALIAS = 0x00000200;
            static inline constexpr MemoryFlags HOST_ACCESS_SEQUENTIAL_WRITE = 0x00000400;
            static inline constexpr MemoryFlags HOST_ACCESS_RANDOM = 0x00000800;
            static inline constexpr MemoryFlags STRATEGY_MIN_MEMORY = 0x00010000;
            static inline constexpr MemoryFlags STRATEGY_MIN_TIME = 0x00020000;
        };

        enum struct CompareOp {
            NEVER = 0,
            LESS = 1,
            EQUAL = 2,
            LESS_OR_EQUAL = 3,
            GREATER = 4,
            NOT_EQUAL = 5,
            GREATER_OR_EQUAL = 6,
            ALWAYS = 7,
        };

        enum struct PolygonMode {
            FILL = 0,
            LINE = 1,
            POINT = 2,
        };

        using FaceCullFlags = u32;
        struct FaceCullFlagBits {
            static inline constexpr FaceCullFlags NONE = 0;
            static inline constexpr FaceCullFlags FRONT_BIT = 0x00000001;
            static inline constexpr FaceCullFlags BACK_BIT = 0x00000002;
            static inline constexpr FaceCullFlags FRONT_AND_BACK = 0x00000003;
        };

        enum struct BlendFactor {
            ZERO = 0,
            ONE = 1,
            SRC_COLOR = 2,
            ONE_MINUS_SRC_COLOR = 3,
            DST_COLOR = 4,
            ONE_MINUS_DST_COLOR = 5,
            SRC_ALPHA = 6,
            ONE_MINUS_SRC_ALPHA = 7,
            DST_ALPHA = 8,
            ONE_MINUS_DST_ALPHA = 9,
            CONSTANT_COLOR = 10,
            ONE_MINUS_CONSTANT_COLOR = 11,
            CONSTANT_ALPHA = 12,
            ONE_MINUS_CONSTANT_ALPHA = 13,
            SRC_ALPHA_SATURATE = 14,
            SRC1_COLOR = 15,
            ONE_MINUS_SRC1_COLOR = 16,
            SRC1_ALPHA = 17,
            ONE_MINUS_SRC1_ALPHA = 18,
        };

        enum struct BlendOp {
            ADD = 0,
            SUBTRACT = 1,
            REVERSE_SUBTRACT = 2,
            MIN = 3,
            MAX = 4,
        };

        using ColorComponentFlags = u32;
        struct ColorComponentFlagBits {
            static inline constexpr ColorComponentFlags R = 0x00000001;
            static inline constexpr ColorComponentFlags G = 0x00000002;
            static inline constexpr ColorComponentFlags B = 0x00000004;
            static inline constexpr ColorComponentFlags A = 0x00000008;
        };

        struct BlendInfo {
            u32 blend_enable = false;
            BlendFactor src_color_blend_factor = BlendFactor::ONE;
            BlendFactor dst_color_blend_factor = BlendFactor::ZERO;
            BlendOp color_blend_op = BlendOp::ADD;
            BlendFactor src_alpha_blend_factor = BlendFactor::ONE;
            BlendFactor dst_alpha_blend_factor = BlendFactor::ZERO;
            BlendOp alpha_blend_op = BlendOp::ADD;
            ColorComponentFlags color_write_mask = ColorComponentFlagBits::R | ColorComponentFlagBits::G | ColorComponentFlagBits::B | ColorComponentFlagBits::A;
        };

        enum ShaderType {
            VERTEX = 0,
            FRAGMENT = 1,
            GEOMETRY = 2,
            COMPUTE
        };
    }
}