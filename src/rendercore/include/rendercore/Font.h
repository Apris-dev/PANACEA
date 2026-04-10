#pragma once

#include "VulkanResources.h"

struct SFont {

	struct Letter {
		Vector2f mUV0, mUV1;
		Extent32 mBearing;
		int64 mAdvance = 0;

		friend COutputArchive& operator<<(COutputArchive& inArchive, const Letter& inLetter) {
			inArchive << inLetter.mUV0 << inLetter.mUV1;
			inArchive << inLetter.mBearing;
			inArchive << inLetter.mAdvance;
			return inArchive;
		}

		friend CInputArchive& operator>>(CInputArchive& inArchive, Letter& inLetter) {
			inArchive >> inLetter.mUV0 >> inLetter.mUV1;
			inArchive >> inLetter.mBearing;
			inArchive >> inLetter.mAdvance;
			return inArchive;
		}
	};

	EXPORT void forEachLetter(const std::string& text, std::function<void(const Vector2f& pos, const Vector2f& uv0, const Vector2f& uv1)> f);

	std::string mName;

	TMap<uint8, Letter> letters;

	TUnique<SVRIImage> mAtlasImage = nullptr;

	Extent32u mAtlasSize = {0, 0};
	int mSize = 0;
	float mLineSpacing = 0.f;
	float mAscenderPx = 0.f;
	float mDescenderPx = 0.f;

	friend COutputArchive& operator<<(COutputArchive& inArchive, const SFont& inFont) {
		inArchive << inFont.mName;
		inArchive << inFont.mAtlasSize;
		inArchive << inFont.mSize;
		inArchive << inFont.mLineSpacing;
		inArchive << inFont.mAscenderPx;
		inArchive << inFont.mDescenderPx;
		inArchive << inFont.letters;
		return inArchive;
	}

	friend CInputArchive& operator>>(CInputArchive& inArchive, SFont& inFont) {
		inArchive >> inFont.mName;
		inArchive >> inFont.mAtlasSize;
		inArchive >> inFont.mSize;
		inArchive >> inFont.mLineSpacing;
		inArchive >> inFont.mAscenderPx;
		inArchive >> inFont.mDescenderPx;
		inArchive >> inFont.letters;
		return inArchive;
	}
};
