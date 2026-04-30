#include "VRI/resources/Shader.h"

#include <combaseapi.h>
#include <wrl.h>
#include <directx-dxc/dxcapi.h>

#include "basic/core/Paths.h"
#include "sarch/FileArchive.h"

using namespace Microsoft::WRL;

#define DXC_CHECK(n) \
	if (FAILED(n)) { \
		errs("Error compiling shader at {}", #n); \
	}

uint32 SShader::compile() {
	// Initialize DXC utils
	ComPtr<IDxcUtils> pUtils;
	DXC_CHECK(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(pUtils.GetAddressOf())));

	// Initialize DXC compiler
	ComPtr<IDxcCompiler3> pCompiler;
	DXC_CHECK(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(pCompiler.GetAddressOf())));

	// Load the HLSL text shader from disk
	ComPtr<IDxcBlobEncoding> pSourceBlob;
	DXC_CHECK(pUtils->CreateBlob(mShaderCode.data(), mShaderCode.size(), CP_UTF8, pSourceBlob.GetAddressOf()));

	// Select correct compile target type
	LPCWSTR targetProfile;
	switch (mStage) {
		case EShaderStage::VERTEX:
			targetProfile = L"vs_6_6";
			break;
		case EShaderStage::FRAGMENT:
			targetProfile = L"ps_6_6";
			break;
		case EShaderStage::COMPUTE:
			targetProfile = L"cs_6_6";
			break;
		default:
			errs("Could not find shader type for shader {}", mFileName);
	}

	/*std::wstring ws;
	ws.resize(inShader.mFileName.size(), L'#');

	size_t outsize;
	mbstowcs_s(&outsize, &ws[0], inShader.mFileName.size(), inShader.mFileName.c_str(), inShader.mFileName.size());
*/
	// Tell it to start at main function, with a target type defined above, to SPIRV
	TVector<LPCWSTR> args {
		L"Temp Filename", //ws.c_str(),
		L"-E", L"main",
		L"-T", targetProfile,
		L"-spirv"
	};

	// Compile shader
	const DxcBuffer buffer{
		.Ptr = pSourceBlob->GetBufferPointer(),
		.Size = pSourceBlob->GetBufferSize(),
		.Encoding = DXC_CP_ACP
	};

	ComPtr<IDxcResult> result{ nullptr };
	HRESULT hres = pCompiler->Compile(
		&buffer,
		args.data(),
		static_cast<uint32_t>(args.getSize()),
		nullptr,
		IID_PPV_ARGS(result.GetAddressOf()));

	// Output error if compilation failed
	if (FAILED(hres) && (result)) {
		ComPtr<IDxcBlobEncoding> errorBlob;
		hres = result->GetErrorBuffer(errorBlob.GetAddressOf());
		if (SUCCEEDED(hres) && errorBlob) {
			errs("Shader Compilation Failed! Reason:\n\n {}", static_cast<const char*>(errorBlob->GetBufferPointer()));
		}
		errs("Shader Compilation Failed for Unknown Reason!");
	}

	// Get compiled code and set mCompiledShader to the result
	ComPtr<IDxcBlob> code;
	DXC_CHECK(result->GetResult(code.GetAddressOf()));

	mCompiledShader.resize(code->GetBufferSize() / sizeof(uint32));
	memcpy(mCompiledShader.data(), code->GetBufferPointer(), code->GetBufferSize());

	return mCompiledShader.getSize();
}

std::string readShaderFile(const char* inFileName) {
	const CFileArchive<EOpenType::READ> file(inFileName);

	if (!file.isOpen()) {
		msgs("I/O error. Cannot open shader file '{}'", inFileName);
		return {};
	}

	std::string code = file.readFile(true);

	// Process includes
	while (code.find("#include ") != std::string::npos) {
		const auto pos = code.find("#include ");
		const auto p1 = code.find('\"', pos);
		const auto p2 = code.find('\"', p1 + 1);
		if (p1 == std::string::npos || p2 == std::string::npos || p2 <= p1) {
			msgs("Error while loading shader program: {}", code.c_str());
			return {};
		}
		const std::string name = code.substr(p1 + 1, p2 - p1 - 1);
		const std::string include = readShaderFile((SPaths::get()->mShaderPath.string() + name.c_str()).c_str());
		code.replace(pos, p2-pos+1, include.c_str());
	}

	return code;
}

bool SShader::loadShader(const char* inFileName, const uint32 Hash) {
	const CFileArchive<EOpenType::BINARY_READ> file(inFileName);

	if (!file.isOpen()) {
		return false;
	}

	TVector<uint32> code = file.readFile<uint32>();

	// The first uint32 value is the hash, if it does not equal the hash for the shader code, it means the shader has changed
	if (code[0] != Hash) {
		msgs("Shader file {} has changed, recompiling.", inFileName);
		return false;
	}

	// Remove the hash so it doesnt mess up the SPIRV shader
	code.popAt(0);

	// Create a new shader module, using the buffer we loaded
	VkShaderModuleCreateInfo createInfo {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		// CodeSize has to be in bytes, so multply the ints in the buffer by size of
		.codeSize = code.getSize() * sizeof(uint32),
		.pCode = code.data()
	};

	// Check that the creation goes well.
	if (vkCreateShaderModule(CVRI::get()->getDevice()->device, &createInfo, nullptr, &mModule) != VK_SUCCESS) {
		return false;
	}
	return true;
}

bool SShader::saveShader(const char* inFileName, const uint32 Hash) const {
	CFileArchive<EOpenType::BINARY_WRITE> file(inFileName);

	// Make sure the file is open
	if (!file.isOpen()) {
		return false;
	}

	TVector<uint32> data = mCompiledShader;

	// Add the hash to the first part of the shader
	data.push(0, Hash);

	file.writeFile(data);

	msgs("Compiled Shader {}.", inFileName);

	return true;
}

SShader::SShader(const char* inFilePath) {
	const std::string fileExtension = std::filesystem::path(inFilePath).extension().string();

	if (fileExtension == ".comp") {
		mStage = EShaderStage::COMPUTE;
	} else if (fileExtension == ".vert") {
		mStage = EShaderStage::VERTEX;
	} else if (fileExtension == ".frag") {
		mStage = EShaderStage::FRAGMENT;
	}

	const std::string path = SPaths::get()->mShaderPath.string() + inFilePath;
	const std::string SPIRVpath = path + ".spv";

	// Get the hash of the original source file so we know if it changed
	const auto shaderSource = readShaderFile(path.c_str());

	if (shaderSource.empty()) {
		errs("Nothing found in Shader file {}!", inFilePath);
	}

	const uint32 Hash = getHash(shaderSource);
	if (Hash == 0) {
		errs("Hash from file {} is not valid.", inFilePath);
	}

	// Check for written SPIRV files
	if (loadShader(SPIRVpath.c_str(), Hash)) {
		return;
	}

	mShaderCode = shaderSource;
	const uint32 result = compile();
	// Save compiled shader
	if (!saveShader(SPIRVpath.c_str(), Hash)) {
		errs("Shader file {} failed to save to {}!", inFilePath, SPIRVpath.c_str());
	}

	// This means the shader didn't compile properly
	if (!result) {
		errs("Shader file {} failed to compile!", inFilePath);
	}

	if (loadShader(SPIRVpath.c_str(), Hash)) {
		return;
	}

	errs("Shader file {} could not be loaded!", inFilePath);
}

std::function<void()> SShader::getDestroyer() {
	return [module = mModule] {
		vkDestroyShaderModule(CVRI::get()->getDevice()->device, module, nullptr);
	};
}