################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/GRT/ClassificationModules/MinDist/MinDist.cpp \
../src/GRT/ClassificationModules/MinDist/MinDistModel.cpp 

OBJS += \
./src/GRT/ClassificationModules/MinDist/MinDist.o \
./src/GRT/ClassificationModules/MinDist/MinDistModel.o 

CPP_DEPS += \
./src/GRT/ClassificationModules/MinDist/MinDist.d \
./src/GRT/ClassificationModules/MinDist/MinDistModel.d 


# Each subdirectory must supply rules for building sources it contributes
src/GRT/ClassificationModules/MinDist/%.o: ../src/GRT/ClassificationModules/MinDist/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


