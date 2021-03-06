#ifdef USE_CUDNN
#include <algorithm>
#include <cfloat>
#include <vector>

#include "thrust/device_vector.h"

#include "caffe/layer.hpp"
#include "caffe/util/math_functions.hpp"
#include "caffe/vision_layers.hpp"

namespace caffe {

template <typename Dtype>
void CuDNNSoftmaxLayer<Dtype>::LayerSetUp(const vector<Blob<Dtype>*>& bottom,
    vector<Blob<Dtype>*>* top, const bool init_ps, int* num_tables,
    map<string, vector<int> >* layer_name_to_blob_global_idx) {
  SoftmaxLayer<Dtype>::LayerSetUp(bottom, top, init_ps, num_tables,
      layer_name_to_blob_global_idx);
  // Initialize CUDNN.
  CUDNN_CHECK(cudnnCreate(&handle_));
  cudnn::createTensor4dDesc<Dtype>(&bottom_desc_);
  cudnn::createTensor4dDesc<Dtype>(&top_desc_);
  handles_setup_ = true;
}

template <typename Dtype>
void CuDNNSoftmaxLayer<Dtype>::Reshape(const vector<Blob<Dtype>*>& bottom,
      vector<Blob<Dtype>*>* top) {
  SoftmaxLayer<Dtype>::Reshape(bottom, top);
  int N = bottom[0]->num();
  int K = bottom[0]->channels();
  int H = bottom[0]->height();
  int W = bottom[0]->width();
  cudnn::setTensor4dDesc<Dtype>(&bottom_desc_, N, K, H, W);
  cudnn::setTensor4dDesc<Dtype>(&top_desc_, N, K, H, W);
}

template <typename Dtype>
CuDNNSoftmaxLayer<Dtype>::~CuDNNSoftmaxLayer() {
  // Check that handles have been setup before destroying
  if (!handles_setup_) { return; }
  cudnnDestroyTensorDescriptor(bottom_desc_);
  cudnnDestroyTensorDescriptor(top_desc_);
  cudnnDestroy(handle_);
}

INSTANTIATE_CLASS(CuDNNSoftmaxLayer);

}  // namespace caffe
#endif
