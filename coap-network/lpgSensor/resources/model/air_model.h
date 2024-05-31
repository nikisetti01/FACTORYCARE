


    // !!! This file is generated using emlearn !!!

    #include <eml_trees.h>
    

static const EmlTreesNode air_model_nodes[41] = {
  { 3, 0.020950f, 1, -1 },
  { 3, 0.017592f, -2, -3 },
  { 1, 54.250000f, 1, 4 },
  { 0, 0.005126f, 1, -1 },
  { 0, 0.004094f, -2, 1 },
  { 3, 0.020950f, -3, -1 },
  { 3, 0.017592f, -2, 1 },
  { 0, 0.005130f, -3, -1 },
  { 0, 0.004090f, 1, 2 },
  { 3, 0.017592f, -2, -3 },
  { 1, 53.549999f, 1, 2 },
  { 3, 0.020950f, -3, -1 },
  { 3, 0.020953f, -3, -1 },
  { 3, 0.017595f, -2, 1 },
  { 3, 0.020950f, -3, -1 },
  { 0, 0.004090f, -2, 1 },
  { 1, 53.549999f, 1, 2 },
  { 3, 0.020950f, -3, -1 },
  { 3, 0.020965f, -3, -1 },
  { 3, 0.017592f, -2, 1 },
  { 3, 0.020950f, -3, -1 },
  { 0, 0.004090f, 1, 4 },
  { 0, 0.004087f, -2, 1 },
  { 1, 75.649998f, -2, 1 },
  { 3, 0.017592f, -2, -3 },
  { 1, 53.549999f, 1, 5 },
  { 2, 0.500000f, 1, 2 },
  { 3, 0.020950f, -3, -1 },
  { 0, 0.005126f, 1, -1 },
  { 3, 0.020950f, -3, -1 },
  { 0, 0.005127f, -3, -1 },
  { 3, 0.017592f, -2, 1 },
  { 3, 0.020950f, -3, -1 },
  { 0, 0.004090f, -2, 1 },
  { 3, 0.020950f, -3, -1 },
  { 3, 0.017592f, -2, 1 },
  { 1, 53.650000f, 1, 4 },
  { 2, 0.500000f, 1, 2 },
  { 3, 0.020950f, -3, -1 },
  { 3, 0.020950f, -3, -1 },
  { 0, 0.005130f, -3, -1 } 
};

static const int32_t air_model_tree_roots[10] = { 0, 2, 8, 13, 15, 19, 21, 31, 33, 35 };

static const uint8_t air_model_leaves[3] = { 2, 0, 1 };

EmlTrees air_model = {
        41,
        (EmlTreesNode *)(air_model_nodes),	  
        10,
        (int32_t *)(air_model_tree_roots),
        3,
        (uint8_t *)(air_model_leaves),
        0,
        4,
        3,
    };

static inline int32_t air_model_tree_0(const float *features, int32_t features_length) {
          if (features[3] < 0.020950f) {
              if (features[3] < 0.017592f) {
                  return 0;
              } else {
                  return 1;
              }
          } else {
              return 2;
          }
        }
        

static inline int32_t air_model_tree_1(const float *features, int32_t features_length) {
          if (features[1] < 54.250000f) {
              if (features[0] < 0.005126f) {
                  if (features[0] < 0.004094f) {
                      return 0;
                  } else {
                      if (features[3] < 0.020950f) {
                          return 1;
                      } else {
                          return 2;
                      }
                  }
              } else {
                  return 2;
              }
          } else {
              if (features[3] < 0.017592f) {
                  return 0;
              } else {
                  if (features[0] < 0.005130f) {
                      return 1;
                  } else {
                      return 2;
                  }
              }
          }
        }
        

static inline int32_t air_model_tree_2(const float *features, int32_t features_length) {
          if (features[0] < 0.004090f) {
              if (features[3] < 0.017592f) {
                  return 0;
              } else {
                  return 1;
              }
          } else {
              if (features[1] < 53.549999f) {
                  if (features[3] < 0.020950f) {
                      return 1;
                  } else {
                      return 2;
                  }
              } else {
                  if (features[3] < 0.020953f) {
                      return 1;
                  } else {
                      return 2;
                  }
              }
          }
        }
        

static inline int32_t air_model_tree_3(const float *features, int32_t features_length) {
          if (features[3] < 0.017595f) {
              return 0;
          } else {
              if (features[3] < 0.020950f) {
                  return 1;
              } else {
                  return 2;
              }
          }
        }
        

static inline int32_t air_model_tree_4(const float *features, int32_t features_length) {
          if (features[0] < 0.004090f) {
              return 0;
          } else {
              if (features[1] < 53.549999f) {
                  if (features[3] < 0.020950f) {
                      return 1;
                  } else {
                      return 2;
                  }
              } else {
                  if (features[3] < 0.020965f) {
                      return 1;
                  } else {
                      return 2;
                  }
              }
          }
        }
        

static inline int32_t air_model_tree_5(const float *features, int32_t features_length) {
          if (features[3] < 0.017592f) {
              return 0;
          } else {
              if (features[3] < 0.020950f) {
                  return 1;
              } else {
                  return 2;
              }
          }
        }
        

static inline int32_t air_model_tree_6(const float *features, int32_t features_length) {
          if (features[0] < 0.004090f) {
              if (features[0] < 0.004087f) {
                  return 0;
              } else {
                  if (features[1] < 75.649998f) {
                      return 0;
                  } else {
                      if (features[3] < 0.017592f) {
                          return 0;
                      } else {
                          return 1;
                      }
                  }
              }
          } else {
              if (features[1] < 53.549999f) {
                  if (features[2] < 0.500000f) {
                      if (features[3] < 0.020950f) {
                          return 1;
                      } else {
                          return 2;
                      }
                  } else {
                      if (features[0] < 0.005126f) {
                          if (features[3] < 0.020950f) {
                              return 1;
                          } else {
                              return 2;
                          }
                      } else {
                          return 2;
                      }
                  }
              } else {
                  if (features[0] < 0.005127f) {
                      return 1;
                  } else {
                      return 2;
                  }
              }
          }
        }
        

static inline int32_t air_model_tree_7(const float *features, int32_t features_length) {
          if (features[3] < 0.017592f) {
              return 0;
          } else {
              if (features[3] < 0.020950f) {
                  return 1;
              } else {
                  return 2;
              }
          }
        }
        

static inline int32_t air_model_tree_8(const float *features, int32_t features_length) {
          if (features[0] < 0.004090f) {
              return 0;
          } else {
              if (features[3] < 0.020950f) {
                  return 1;
              } else {
                  return 2;
              }
          }
        }
        

static inline int32_t air_model_tree_9(const float *features, int32_t features_length) {
          if (features[3] < 0.017592f) {
              return 0;
          } else {
              if (features[1] < 53.650000f) {
                  if (features[2] < 0.500000f) {
                      if (features[3] < 0.020950f) {
                          return 1;
                      } else {
                          return 2;
                      }
                  } else {
                      if (features[3] < 0.020950f) {
                          return 1;
                      } else {
                          return 2;
                      }
                  }
              } else {
                  if (features[0] < 0.005130f) {
                      return 1;
                  } else {
                      return 2;
                  }
              }
          }
        }
        

int32_t air_model_predict(const float *features, int32_t features_length) {

        int32_t votes[3] = {0,};
        int32_t _class = -1;

        _class = air_model_tree_0(features, features_length); votes[_class] += 1;
    _class = air_model_tree_1(features, features_length); votes[_class] += 1;
    _class = air_model_tree_2(features, features_length); votes[_class] += 1;
    _class = air_model_tree_3(features, features_length); votes[_class] += 1;
    _class = air_model_tree_4(features, features_length); votes[_class] += 1;
    _class = air_model_tree_5(features, features_length); votes[_class] += 1;
    _class = air_model_tree_6(features, features_length); votes[_class] += 1;
    _class = air_model_tree_7(features, features_length); votes[_class] += 1;
    _class = air_model_tree_8(features, features_length); votes[_class] += 1;
    _class = air_model_tree_9(features, features_length); votes[_class] += 1;
    
        int32_t most_voted_class = -1;
        int32_t most_voted_votes = 0;
        for (int32_t i=0; i<3; i++) {

            if (votes[i] > most_voted_votes) {
                most_voted_class = i;
                most_voted_votes = votes[i];
            }
        }
        return most_voted_class;
    }
    