#ifndef ANN_H
#define ANN_H

#include <stdlib.h>
#include <sys/timeb.h>

typedef unsigned short ushort;
typedef unsigned int uint;
typedef double annreal;

typedef struct ANNSynapse ANNSynapse;
typedef struct ANNNeuron ANNNeuron;
typedef struct ANNLayer ANNLayer;
typedef struct ANNSet ANNSet;
typedef struct ANNet ANNet;

typedef enum
{
    ANN_LINEAR,
    ANN_SIGMOID,
    ANN_SIGMOID_SYMMETRIC
} ANNActivateFunction;

typedef enum {
    ANN_ALL_LAYERS,
    ANN_HIDDEN_LAYERS,
    ANN_OUTPUT_LAYER
} ANNLayerGroup;

typedef enum {
    ANN_STOP_DESIRED_RMSE,
    ANN_STOP_NO_BITFAILS,
    ANN_DONT_STOP
} ANNStopMode;

typedef enum {
    ANN_TRAIN_STANDARD_EBP,
    ANN_TRAIN_RPROP
} ANNTrainAlgorithm;

struct ANNSynapse {
    annreal weight;
    annreal weight_delta;
    annreal rprop_weight_slope;
    annreal rprop_prev_weight_step;
    annreal rprop_prev_weight_slope;
    ANNNeuron *input_neuron;
    ANNNeuron *output_neuron;
};

struct ANNNeuron {
    annreal value;
    annreal delta;
    annreal bias;
    annreal bias_delta;
    annreal rprop_bias_slope;
    annreal rprop_prev_bias_slope;
    annreal rprop_prev_bias_step;
    ushort input_synapses_num;
    ushort output_synapses_num;
    ANNSynapse **input_synapses;
    ANNSynapse **output_synapses;
};

struct ANNLayer {
    ushort neurons_num;
    ANNActivateFunction activate_func;
    ANNNeuron **neurons;
    ANNLayer *prev_layer;
    ANNLayer *next_layer;
};

struct ANNSet {
    annreal *input;
    annreal *output;
};

struct ANNet {
    uint train_num_sets;
    uint random_seed;
    uint bit_fails;
    annreal mse;
    annreal prev_mse;
    annreal bit_fail_limit;
    annreal desired_rmse;
    annreal steepness;
    ANNStopMode stop_mode;
    ANNLayer *input_layer;
    ANNLayer *output_layer;
    ANNSet **train_sets;

    ANNTrainAlgorithm train_algorithm;

    annreal learning_rate;
    annreal momentum;

    annreal rprop_increase_factor;
    annreal rprop_decrease_factor;
    annreal rprop_min_step;
    annreal rprop_max_step;
};

#ifdef __cplusplus
extern "C" {
#endif

void ann_init(ANNet *net);

void ann_add_layer(ANNet *net, int neurons_num);
void ann_add_train_set(ANNet *net, annreal *input, annreal *output);
int ann_load_train_sets(ANNet *net, const char *filename);

void ann_run(ANNet *net, annreal *input, annreal *output);

void ann_train_set(ANNet *net, annreal *input, annreal *output);
void ann_train_sets(ANNet *net);
void ann_train(ANNet *net, annreal max_train_time, annreal report_interval);
void ann_report(ANNet *net, uint epoch);

void ann_calc_errors(ANNet *net);
annreal ann_calc_set_rmse(ANNet *net, annreal *input, annreal *output);

void ann_dump_train_sets(ANNet *net);
void ann_randomize_weights(ANNet *net, annreal min, annreal max);

void ann_set_training_algorithm(ANNet *net, ANNTrainAlgorithm train_algorithm);
void ann_set_rprop_params(ANNet *net, annreal increase_factor, annreal decrease_factor, annreal min_step, annreal max_step);
void ann_set_desired_rmse(ANNet *net, annreal desired_rmse);
void ann_set_bit_fail_limit(ANNet *net, annreal bit_fail_limit);
void ann_set_stop_mode(ANNet *net, ANNStopMode stop_mode);
void ann_set_learning_rate(ANNet *net, annreal learning_rate);
void ann_set_momentum(ANNet *net, annreal momentum);
void ann_set_steepness(ANNet *net, annreal steepness);
void ann_set_activate_function(ANNet *net, ANNActivateFunction func, ANNLayerGroup layer_group);

/* utilities */
extern inline annreal ann_random_range(annreal min, annreal max)
{ return (min + (((max-min) * rand())/(RAND_MAX + 1.0))); }
extern inline annreal ann_convert_range(annreal v, annreal from_min, annreal from_max, annreal to_min, annreal to_max)
{ return ((((v - from_min) / (from_max - from_min)) * (to_max - to_min)) + to_min); }
extern inline annreal ann_get_seconds()
{ struct timeb t; ftime(&t); return (((annreal)t.millitm/1000.0) + (annreal)t.time); }

#define ann_clip(v,min,max) ((v > max) ? max : ((v < min) ? min : v))
#define ann_min(a,b) (a < b ? a : b)
#define ann_max(a,b) (a > b ? a : b)
#define ann_sign(a) (a > 0 ? 1 : (a < 0 ? -1 : 0))

#ifdef __cplusplus
};
#endif

#endif
