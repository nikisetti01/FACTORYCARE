#include "air_model.h"

int predict_class(float* features, int len) {

      const int32_t predicted_class2 = eml_trees_predict(&air_model,features, len);

      int value_predicted = (int)predicted_class2;

      printf("prediction2 %i\n", value_predicted);

    return value_predicted;
}