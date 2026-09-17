# NoC-Based CNN Accelerator with AXI4 DMA

An independent course project for **Machine Learning Chip Design**. The project uses SystemC to model AlexNet inference and progressively explores modular communication, a 4x4 mesh Network-on-Chip (NoC), and a DRAM-based accelerator memory system with AXI4-style DMA transfers.

## Project progression

1. **AlexNet inference in SystemC** - implemented convolution, ReLU, max-pooling, fully connected, and softmax stages.
2. **SystemC communication channels** - evaluated `sc_signal`, `sc_buffer`, and `sc_fifo` for communication between modules.
3. **4x4 mesh NoC** - modeled 16 routers/cores, packet-to-flit conversion, FIFO buffering, and deterministic XY routing.
4. **NoC-based CNN architecture** - integrated CNN computation with the mesh communication architecture.
5. **Final project** - replaced ROM-style access with external DRAM and an AXI4-style DMA model, then added memory-traffic and compute-resource instrumentation.

## Final-project architecture

- SystemC-based AlexNet inference model
- 4x4 mesh NoC concepts inherited from the course sequence
- Word-addressable DRAM behavioral model
- AXI4-style DMA with a maximum burst length of 256 words
- Explicit memory regions for images, weights, biases, intermediate values, and outputs
- Output write-back to DRAM followed by DMA read-back for classification
- Compute model with 16 processing elements and 16 modeled MAC units per PE
- Optimized variant with a 65,536-word (256 KiB) SRAM behavioral model
- Statistics for DRAM/SRAM traffic, DMA transactions, MAC operations, and estimated cycles

## Functional verification

The baseline and optimized variants preserved the expected top-1 predictions:

| Test input | Expected top-1 result | Confidence |
| --- | --- | ---: |
| Cat | Egyptian cat | 96.381% |
| Dog | Golden retriever | 38.628% |

These values are classification confidences for the supplied test inputs, not dataset-level accuracy.

## Repository structure

```text
.
├── reports/
│   ├── HW1_AlexNet_SystemC.pdf
│   ├── HW2_SystemC_Channels.pdf
│   ├── HW3_4x4_Mesh_NoC.pdf
│   ├── HW4_NoC_Based_CNN.pdf
│   └── Final_Project_Report.pdf
└── src/
    ├── baseline/
    └── optimized/
```

`src/baseline` contains the DRAM/DMA baseline. `src/optimized` adds the SRAM behavioral model and related statistics.

## Build requirements

- GNU C++ compiler
- SystemC 2.3.3
- Course-provided AlexNet parameters, ImageNet class labels, and cat/dog input files

The supplied Makefiles reference the original course-server SystemC installation. Update `HOME`, `LIB_DIR`, and `INC_DIR` in each Makefile for a different environment.

Example commands after configuring the SystemC paths and placing the required files in `data/`:

```bash
cd src/baseline
make
make cat
make dog
```

## Important limitations

- The simulator is a transaction/system-level model, not an RTL implementation or fabricated accelerator.
- Cycle values are analytical estimates based on the modeled DMA, SRAM, and MAC resources; they are not measurements from synthesized hardware.
- The optimized design improves memory-hierarchy modeling and observability. It does not claim to reduce off-chip DRAM traffic in the current implementation.
- The provided final-project archive contains the baseline and optimized final source snapshots. Earlier NoC stages are documented in the reports, but their separate historical source snapshots are not included here.
- Course handouts, grading scripts, pretrained parameters, and test data are intentionally excluded.

## Academic-use notice

This repository documents completed coursework. Do not copy or submit this work for another course assignment. Course-provided materials remain the property of their respective authors.
