#include "simple_compute.h"
#include <chrono>
#include <memory>

template<typename CollableT, typename... ArgsT>
float ExecutionTime(CollableT func, ArgsT&&... args) {
  using timer = std::chrono::high_resolution_clock;
  auto start = timer::now();
  func(std::forward<ArgsT>(args)...);
  auto end = timer::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

void ComputeOnCpu(const std::vector<float>& input) {
  std::vector<float> answer(input.size());
  float sum = 0.0f;
  size_t n = input.size();

  for (size_t i = 0; i < n; ++i) {
    answer[i] = input[i];
    for (size_t j = 1; j <= 3 && i >= j; ++j) {
      answer[i] += input[i - j];
    }
    for (size_t j = 1; j <= 3 && i + j < n; ++j) {
      answer[i] += input[i - j];
    }
    answer[i] /= 7.f;
    answer[i] -= input[i];
    sum += answer[i];
  }

  std::cout << "CPU result: " << sum << '\n';
}

void ComputeOnGpu(SimpleCompute& device, const std::vector<float>& input) {
  size_t n = input.size();

  auto helper = device.GetCopyHelper();
  helper->UpdateBuffer(device.GetInputBuffer(), 0, input.data(), sizeof(float) * n);

  device.Execute();

  std::vector<float> output(n);
  helper->ReadBuffer(device.GetOutputBuffer(), 0, output.data(), sizeof(float) * n);

  float sum = 0.f;
  for (auto x : output)
    sum += x;

  std::cout << "GPU result: " << sum << '\n';
}

float GenRandom(float mod) {
  srand(time(nullptr));
  return (float)(rand()) / (float)(RAND_MAX / mod);
}

std::vector<float> GenRandomArray(size_t n, float mod) {
  std::vector<float> array(n);
  for (auto& x : array)
    x = GenRandom(mod);
  return array;
}

int main() {
  constexpr int LENGTH = 1000000;
  constexpr int VULKAN_DEVICE_ID = 0;

  auto device = std::make_unique<SimpleCompute>(LENGTH);
  if (device == nullptr) {
    std::cout << "Can't create render of specified type" << std::endl;
    return 1;
  }

  device->InitVulkan(nullptr, 0, VULKAN_DEVICE_ID);
  device->InitPipeline();

  auto input = GenRandomArray(LENGTH, 10);

  std::cout << ExecutionTime(ComputeOnCpu, input) << '\n';
  std::cout << ExecutionTime(ComputeOnGpu, *device, input) << '\n';

  return 0;
}
