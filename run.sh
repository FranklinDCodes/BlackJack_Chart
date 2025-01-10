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
MAX_TRAINING_CHART_NAME="Chart${CHART_ID}_backing.csv"
Q_CHART_NAME="Chart${CHART_ID}_Q.csv"
TRAINING_CHART_NAME="Chart${CHART_ID}_wholeBacking.csv"
PARAM_FILE_NAME="${CHART_ID}_parameters.txt"

# move all result files
mv ${MAX_TRAINING_CHART_NAME} Chart${runNum}/${MAX_TRAINING_CHART_NAME}
mv ${Q_CHART_NAME} Chart${runNum}/${Q_CHART_NAME}
mv ${TRAINING_CHART_NAME} Chart${runNum}/${TRAINING_CHART_NAME}
mv ${PARAM_FILE_NAME} Chart${runNum}/${PARAM_FILE_NAME}

# delete executable
rm driver.exe
