# Neuro Lib - Multilayer Perceptron Neural Network Library

A comprehensive Python library for building, training, and deploying multilayer perceptron (MLP) neural networks. Neuro Lib provides an intuitive API for creating deep learning models with full control over architecture, activation functions, and optimization strategies.

## Features

- **Flexible Architecture**: Build custom neural networks with multiple layers and configurable neuron counts
- **Multiple Activation Functions**: Support for ReLU, Sigmoid, Tanh, Linear, and Softmax activations
- **Advanced Optimizers**: Implementations of SGD, Adam, RMSprop, and Momentum-based optimization
- **Loss Functions**: Cross-entropy, Mean Squared Error (MSE), Binary Cross-entropy, and custom loss support
- **Batch Processing**: Efficient mini-batch training for improved convergence and memory management
- **Regularization**: L1 and L2 regularization to prevent overfitting
- **Easy Model Serialization**: Save and load trained models for inference and transfer learning
- **Comprehensive Logging**: Track training metrics and model performance in real-time
- **NumPy-Based**: Pure Python implementation with NumPy for efficiency and compatibility
- **Well-Documented API**: Clear documentation and examples for rapid development

## Installation

### Requirements

- Python 3.7 or higher
- NumPy >= 1.19.0
- Matplotlib >= 3.3.0 (optional, for visualization)
- Pandas >= 1.1.0 (optional, for data handling)

### Via pip

```bash
pip install neuro-lib
```

### From Source

```bash
git clone https://github.com/NikkiRe/neuro_lib.git
cd neuro_lib
pip install -e .
```

### Development Installation

```bash
git clone https://github.com/NikkiRe/neuro_lib.git
cd neuro_lib
pip install -e ".[dev]"
```

## Quick Start

### Basic Example: Binary Classification

```python
from neuro_lib import NeuralNetwork, Layer, Activations, Optimizers
import numpy as np

# Create a neural network
model = NeuralNetwork(random_state=42)

# Add layers
model.add(Layer(input_size=2, output_size=64, activation=Activations.ReLU))
model.add(Layer(input_size=64, output_size=32, activation=Activations.ReLU))
model.add(Layer(input_size=32, output_size=1, activation=Activations.Sigmoid))

# Compile the model
model.compile(
    optimizer=Optimizers.Adam(learning_rate=0.001),
    loss='binary_crossentropy',
    metrics=['accuracy']
)

# Train the model
X_train = np.random.randn(100, 2)
y_train = np.random.randint(0, 2, (100, 1))

history = model.fit(
    X_train, y_train,
    epochs=50,
    batch_size=16,
    validation_split=0.2,
    verbose=True
)

# Make predictions
X_test = np.random.randn(20, 2)
predictions = model.predict(X_test)
```

### Multi-Class Classification Example

```python
from neuro_lib import NeuralNetwork, Layer, Activations

# Create a model for 3-class classification
model = NeuralNetwork()

model.add(Layer(input_size=10, output_size=128, activation=Activations.ReLU))
model.add(Layer(input_size=128, output_size=64, activation=Activations.ReLU))
model.add(Layer(input_size=64, output_size=3, activation=Activations.Softmax))

model.compile(
    optimizer='adam',
    loss='categorical_crossentropy',
    metrics=['accuracy']
)

# Train on multi-class data
X_train = np.random.randn(200, 10)
y_train = np.eye(3)[np.random.randint(0, 3, 200)]  # One-hot encoded

model.fit(X_train, y_train, epochs=100, batch_size=32)

# Get class predictions
predictions = model.predict(X_test)
class_labels = np.argmax(predictions, axis=1)
```

## Architecture

### Network Components

#### Layer
The fundamental building block of the network. Each layer transforms input data through:
1. **Weight Matrix**: Learnable parameters (W)
2. **Bias Vector**: Learnable offsets (b)
3. **Activation Function**: Non-linear transformation

```python
Layer(input_size, output_size, activation='relu', use_bias=True)
```

#### Activation Functions

- **ReLU** (Rectified Linear Unit): `f(x) = max(0, x)` - Default choice for hidden layers
- **Sigmoid**: `f(x) = 1 / (1 + e^-x)` - For binary classification
- **Tanh**: `f(x) = (e^x - e^-x) / (e^x + e^-x)` - Similar to sigmoid, range [-1, 1]
- **Linear**: `f(x) = x` - For regression tasks
- **Softmax**: `f(x) = e^x / Σe^x` - For multi-class classification

#### Optimizers

- **SGD** (Stochastic Gradient Descent): Basic gradient descent with momentum
- **Adam**: Adaptive Moment Estimation - recommended for most problems
- **RMSprop**: Root Mean Square Propagation - good for non-stationary problems
- **Momentum**: Standard momentum-based gradient descent

#### Loss Functions

- **Binary Cross-Entropy**: For binary classification
- **Categorical Cross-Entropy**: For multi-class classification
- **Mean Squared Error (MSE)**: For regression tasks
- **Sparse Categorical Cross-Entropy**: For multi-class with integer labels

### Training Flow

```
Input Data
    ↓
Forward Pass (through all layers)
    ↓
Calculate Loss
    ↓
Backward Pass (compute gradients)
    ↓
Update Weights (using optimizer)
    ↓
Repeat for n epochs
    ↓
Trained Model
```

## Examples

### Example 1: Regression Task (Predicting House Prices)

```python
from neuro_lib import NeuralNetwork, Layer, Activations

# Create regression model
model = NeuralNetwork()
model.add(Layer(10, 64, activation=Activations.ReLU))
model.add(Layer(64, 32, activation=Activations.ReLU))
model.add(Layer(32, 1, activation=Activations.Linear))  # Linear for regression

model.compile(optimizer='adam', loss='mse', metrics=['mae'])

# Train
X_train = np.random.randn(300, 10)
y_train = np.random.randn(300, 1)

model.fit(X_train, y_train, epochs=100, batch_size=32)

# Predict
y_pred = model.predict(X_test)
```

### Example 2: Deep Network with Regularization

```python
from neuro_lib import NeuralNetwork, Layer, Regularizers

model = NeuralNetwork()
model.add(Layer(784, 256, activation='relu', regularizer=Regularizers.L2(0.001)))
model.add(Layer(256, 128, activation='relu', regularizer=Regularizers.L2(0.001)))
model.add(Layer(128, 64, activation='relu', regularizer=Regularizers.L2(0.001)))
model.add(Layer(64, 10, activation='softmax'))

model.compile(
    optimizer='adam',
    loss='categorical_crossentropy',
    metrics=['accuracy']
)

history = model.fit(X_train, y_train, epochs=200, batch_size=128, validation_split=0.2)
```

### Example 3: Model Serialization and Loading

```python
# Save trained model
model.save('my_model.pkl')

# Load model later
from neuro_lib import load_model
loaded_model = load_model('my_model.pkl')

# Use loaded model for inference
predictions = loaded_model.predict(new_data)
```

### Example 4: Custom Training Loop

```python
model = NeuralNetwork()
model.add(Layer(20, 64, activation='relu'))
model.add(Layer(64, 32, activation='relu'))
model.add(Layer(32, 1, activation='sigmoid'))

model.compile(optimizer='sgd', loss='binary_crossentropy')

# Manual training loop for fine-grained control
for epoch in range(50):
    batch_losses = []
    for X_batch, y_batch in create_batches(X_train, y_train, batch_size=32):
        loss = model.train_on_batch(X_batch, y_batch)
        batch_losses.append(loss)
    
    avg_loss = np.mean(batch_losses)
    print(f"Epoch {epoch+1}: Loss = {avg_loss:.4f}")
```

## API Reference

### NeuralNetwork Class

#### Methods

**`__init__(random_state=None, input_validation=True)`**
- Initialize a new neural network
- `random_state`: Seed for reproducibility
- `input_validation`: Enable input shape validation

**`add(layer)`**
- Add a layer to the network
- `layer`: Layer object to add

**`compile(optimizer, loss, metrics=None)`**
- Configure the model for training
- `optimizer`: Optimizer instance or string ('adam', 'sgd', 'rmsprop')
- `loss`: Loss function ('binary_crossentropy', 'mse', 'categorical_crossentropy')
- `metrics`: List of metrics to track during training

**`fit(X, y, epochs=1, batch_size=32, validation_split=0.0, verbose=True)`**
- Train the network
- `X`: Training input data (n_samples, n_features)
- `y`: Training target data (n_samples, n_outputs)
- `epochs`: Number of training epochs
- `batch_size`: Mini-batch size
- `validation_split`: Fraction of data to use for validation
- Returns: Training history dictionary

**`predict(X, batch_size=32)`**
- Make predictions on new data
- `X`: Input data
- Returns: Predictions

**`evaluate(X, y, batch_size=32)`**
- Evaluate model on test data
- Returns: Loss and metrics

**`save(filepath)`**
- Save trained model to file

**`load(filepath)`**
- Load a previously saved model

### Layer Class

```python
Layer(
    input_size,
    output_size,
    activation='relu',
    use_bias=True,
    kernel_initializer='he_normal',
    bias_initializer='zeros',
    regularizer=None
)
```

**Parameters:**
- `input_size`: Number of input features
- `output_size`: Number of neurons in this layer
- `activation`: Activation function to use
- `use_bias`: Whether to include bias terms
- `kernel_initializer`: Weight initialization method
- `bias_initializer`: Bias initialization method
- `regularizer`: Regularization to apply

### Activation Functions

```python
from neuro_lib import Activations

Activations.ReLU
Activations.Sigmoid
Activations.Tanh
Activations.Linear
Activations.Softmax
```

### Optimizers

```python
from neuro_lib import Optimizers

# Instantiate optimizers
optimizer = Optimizers.Adam(learning_rate=0.001, beta1=0.9, beta2=0.999)
optimizer = Optimizers.SGD(learning_rate=0.01, momentum=0.9)
optimizer = Optimizers.RMSprop(learning_rate=0.001, decay=0.9)
```

### Loss Functions

```python
from neuro_lib import Losses

'binary_crossentropy'      # For binary classification
'categorical_crossentropy'  # For multi-class classification
'mse'                      # Mean Squared Error for regression
'sparse_categorical_crossentropy'  # For multi-class with integer labels
```

## Requirements

See `requirements.txt` for all dependencies:

```
numpy>=1.19.0
matplotlib>=3.3.0
pandas>=1.1.0
scipy>=1.5.0
```

### Development Requirements

```
pytest>=6.0.0
pytest-cov>=2.10.0
black>=20.8b1
flake8>=3.8.0
mypy>=0.800
sphinx>=3.3.0
```

## Performance Considerations

- **Batch Size**: Larger batches = faster training but less frequent updates. Typical range: 16-256
- **Learning Rate**: Start with 0.001-0.01, adjust based on convergence
- **Network Depth**: More layers can capture complex patterns but require more data
- **Regularization**: Use L1/L2 regularization if overfitting is observed
- **Activation Functions**: ReLU for hidden layers, Softmax/Sigmoid for output

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Citation

If you use Neuro Lib in your research, please cite it as:

```bibtex
@software{neuro_lib_2024,
  author = {NikkiRe},
  title = {Neuro Lib: Multilayer Perceptron Neural Network Library},
  url = {https://github.com/NikkiRe/neuro_lib},
  year = {2024}
}
```

## Support & Documentation

- **Documentation**: [Read the Docs](https://neuro-lib.readthedocs.io/)
- **Issues**: [GitHub Issues](https://github.com/NikkiRe/neuro_lib/issues)
- **Discussions**: [GitHub Discussions](https://github.com/NikkiRe/neuro_lib/discussions)

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for detailed version history and updates.

---

**Last Updated**: December 29, 2024
**Version**: 1.0.0
