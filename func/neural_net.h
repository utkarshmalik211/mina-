/*
   Author - Utkarsh Malik
   Date - 3rd september
 */

typedef enum LayerType {INPUT, HIDDEN ,OUTPUT} LayerType;

typedef enum ActFctType {SIGMOID, TANH, RELU} ActFctType;

typedef struct Node {
								double bias;
								double output;
								int wcount;
								double weights[];
								// considered as struct hack in c , we wont specify the array element count but will manually allocate space for
} Node;

typedef struct Layer {
								int ncount;
								Node nodes[];
} Layer;

typedef struct Network {
								int inpNodeSize;
								int inpLayerSize;
								int hidNodeSize;
								int hidLayerSize;

								int outNodeSize;
								int outLayerSize;
								ActFctType hidLayerActType;
								ActFctType outLayerActType;
								double learningRate;
								Layer layers[];
} Network;
Layer *createInputLayer(int inpCount){
								int inpNodeSize = sizeof(Node);
								int inpLayerSize= sizeof(Layer) + (inpCount * inpNodeSize);

								Layer *il = malloc(inpLayerSize);
								il->ncount = inpCount;

								Node iln;
								iln.bias = 0;
								iln.output = 0;
								iln.wcount = 0;

								uint8_t *sbptr = (uint8_t*) il->nodes;
								// single byte pointer is simply a pointer pointing to
								// memory blocks of byte size 1. I.e. we can easily
								// move the pointer throughout the address space either by
								// incrementing it via pointer++
								// or by adding the number of bytes that we want

								for (int i = 0; i < il->ncount; i++) {

																memcpy(sbptr,&iln,inpNodeSize); // copy individual node to layer

																sbptr += inpNodeSize;
								}
								return il;
}

Layer *createLayer(int nodeCount,int weightCount){
								int nodeSize = sizeof(Node) + (weightCount * sizeof(double));
								Layer *l = (Layer*)malloc(sizeof(Layer)+(nodeCount*nodeSize));
								l->ncount = nodeCount;

								Node *dn = (Node*)malloc(sizeof(Node) + ((weightCount)*sizeof(double)));
								dn->bias = 0;
								dn->output = 0;
								dn->wcount = weightCount;

								for (int i = 0; i < weightCount; i++) {
																dn->weights[i]=0;
								}

								uint8_t *sbptr = (uint8_t*) l->nodes;

								for (int j = 0; j < nodeCount; j++) {
																memcpy(sbptr+(j*nodeSize),dn,nodeSize);
								}

								free(dn);

								return l;
}



void initNetwork(Network *nn,int inpCount,int hidCount,int outCount){

								Layer *il = createInputLayer(inpCount);
								memcpy(nn->layers,il,nn->inpLayerSize); // copy il's content to nn
								free(il);

								uint8_t *sbptr = (uint8_t*) nn->layers; //single byte pointer for fast access
								sbptr += nn->inpLayerSize; // move pointer to begining of hidden layer

								Layer *hl = createLayer(hidCount, inpCount);
								memcpy(sbptr,hl,nn->hidLayerSize);
								free(hl);

								sbptr += nn->hidLayerSize; // move to begining of hid1 layer

								Layer *ol = createLayer(outCount,hidCount);
								memcpy(sbptr,ol,nn->outLayerSize);
								free(ol);


}

Layer *getLayer(Network *nn,LayerType ltype){
								Layer *l;
								switch (ltype) {
								case INPUT: {
																l=nn->layers;
																break;
								}
								case HIDDEN: {
																uint8_t *sbptr = (uint8_t*)nn->layers;
																sbptr += nn->inpLayerSize;
																l=(Layer*)sbptr;
																break;
								}
								default: {
																uint8_t *sbptr = (uint8_t*)nn->layers;
																sbptr += nn->inpLayerSize + nn->hidLayerSize;
																l=(Layer*)sbptr;
																break;
								}
								}
								return l;
}

Node *getNode(Layer *l, int nodeId) {

								int nodeSize = sizeof(Node) + (l->nodes[0].wcount * sizeof(double));
								uint8_t *sbptr = (uint8_t*) l->nodes;

								sbptr += nodeId * nodeSize;

								return (Node*) sbptr;
}


//random weights initialization

void initWeights(Network *nn, LayerType ltype){

    int nodeSize = 0;
    if (ltype==HIDDEN) nodeSize=nn->hidNodeSize;
                  else nodeSize=nn->outNodeSize;

    Layer *l = getLayer(nn, ltype);

    uint8_t *sbptr = (uint8_t*) l->nodes;

    for (int o=0; o<l->ncount;o++){

        Node *n = (Node *)sbptr;

        for (int i=0; i<n->wcount; i++){
            n->weights[i] = 0.7*(rand()/(double)(RAND_MAX));
            if (i%2) n->weights[i] = -n->weights[i];  // make half of the weights negative
        }

        // init bias weight
        n->bias =  rand()/(double)(RAND_MAX);
        if (o%2) n->bias = -n->bias;  // make half of the bias weights negative

        sbptr += nodeSize;
    }

}
Network *createNetwork(int inpCount,int hidCount,int outCount){
								int inpNodeSize  = sizeof(Node);
								int inpLayerSize = sizeof(Layer) + (inpCount * inpNodeSize);

								int hidWeightsCount = inpCount;
								int hidNodeSize  =  sizeof(Node) + (hidWeightsCount * sizeof(double));
								int hidLayerSize = sizeof(Layer) + (hidCount * hidNodeSize);

								int outWeightsCount = hidCount;
								int outNodeSize  = sizeof(Node) + (outWeightsCount * sizeof(double));
								int outLayerSize = sizeof(Layer) + (outCount * outNodeSize);

								Network *nn = (Network*)malloc(sizeof(Network) + inpLayerSize + hidLayerSize + outLayerSize);

								nn->inpNodeSize   = inpNodeSize;
								nn->inpLayerSize  = inpLayerSize;
								nn->hidNodeSize   = hidNodeSize;
								nn->hidLayerSize  = hidLayerSize;
								nn->outNodeSize   = outNodeSize;
								nn->outLayerSize  = outLayerSize;
								nn->hidLayerActType = SIGMOID;
								nn->outLayerActType = SIGMOID;
								nn->learningRate = 0.1;

								initNetwork(nn, inpCount, hidCount, outCount);

								initWeights(nn,HIDDEN);
								initWeights(nn,OUTPUT);

								return nn;
}
