# utvm

## How to add models

1. add a base folder for the models
2. add that folder to EXTERNAL_PKG_DIRS
3. create one folder per model *as per the model's .tar.
   **NOTE** the tar name and the model name inside tha tar have to match!
4. add Makefile and Makefile.include in each model folder.
   (copy tests/utvm/models/default/Makefile*)
5. add `USEPKG += <model_name>`
