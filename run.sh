#! /bin/bash
# purpose: compiles, runs and reorganizes files for driver

# capture run num
runNum=$1

# create new dir
mkdir Chart${runNum}

# compile cpp prog
g++ driver.cpp -Wall -o driver.exe

# run prog
./driver.exe ${runNum}

# result filepaths
CHART_NAME="Chart${runNum}.csv"
USABLE_CHART_NAME="Chart${runNum}_readable.csv"
MAX_TRAINING_CHART_NAME="Chart${runNum}_backing.csv"
Q_CHART_NAME="Chart${runNum}_Q.csv"
TRAINING_CHART_NAME="Chart${runNum}_wholeBacking.csv"
PARAM_FILE_NAME="${runNum}_parameters.txt"

# move all result files
mv ${CHART_NAME} Chart${runNum}/${CHART_NAME}
mv ${USABLE_CHART_NAME} Chart${runNum}/${USABLE_CHART_NAME}
mv ${MAX_TRAINING_CHART_NAME} Chart${runNum}/${MAX_TRAINING_CHART_NAME}
mv ${Q_CHART_NAME} Chart${runNum}/${Q_CHART_NAME}
mv ${TRAINING_CHART_NAME} Chart${runNum}/${TRAINING_CHART_NAME}
mv ${PARAM_FILE_NAME} Chart${runNum}/${PARAM_FILE_NAME}

# compile evaluation
g++ eval.cpp -Wall -o eval.exe

# run with chart id
./eval.exe ${runNum}

# delete executables
rm driver.exe
rm eval.exe
