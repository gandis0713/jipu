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

class TFLiteInference
{
public:
    TFLiteInference();
    ~TFLiteInference();

    bool setInputImage(std::unique_ptr<Image> image);
    bool loadModel(const std::vector<char>& modelBuffer);
    std::vector<uint8_t> runInference();

private:
    void preprocessImage(std::vector<float>& preprocessed);

    std::vector<uint8_t> postprocessOutput(const float* output,
                                           int outputSize);

private:
    TfLiteInterpreter* m_interpreter{ nullptr };
    TfLiteModel* m_model{ nullptr };
    std::unique_ptr<Image> m_inputImage{ nullptr };
};

} // namespace jipu