#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "file.h"
#include "image.h"

namespace fs = std::filesystem;
struct TfLiteInterpreter;
struct TfLiteModel;
namespace jipu
{

class TFLiteImageInference
{
public:
    TFLiteImageInference();
    ~TFLiteImageInference();

    bool setInputImage(Image* image);
    bool loadModel(const std::vector<char>& modelBuffer);
    std::vector<uint8_t> runInference();

    int32_t getBatchSize();
    int32_t getWidth();
    int32_t getHeight();
    int32_t getInputChannel();
    int32_t getOutputChannel();
    size_t getInputByteSize();
    size_t getInputSize();
    size_t getOutputByteSize();
    size_t getOutputSize();

private:
    void preprocessImage(std::vector<float>& preprocessed);

    std::vector<uint8_t> postprocessOutput(const float* output,
                                           int outputSize);

private:
    TfLiteInterpreter* m_interpreter{ nullptr };
    TfLiteModel* m_model{ nullptr };
    Image* m_inputImage{ nullptr };
};

} // namespace jipu