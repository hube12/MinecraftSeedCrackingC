include(CheckLanguage)

find_package (Threads)

add_executable(Test Random.hpp Parser.cpp Utils.cpp PillarsCracker.cpp StructureCracker.cpp GenerationCracker.cpp generationByCubitect/generator.cpp generationByCubitect/layers.cpp generationByCubitect/finders.cpp Tests.cpp)
target_link_libraries (Test ${CMAKE_THREAD_LIBS_INIT})

add_executable(tt Random.hpp Parser.cpp test.cc)

add_executable(SeedCrackerByNeil2.0 Random.hpp Parser.cpp Utils.cpp PillarsCracker.cpp StructureCracker.cpp GenerationCracker.cpp generationByCubitect/generator.cpp generationByCubitect/layers.cpp StructureCracker.cpp MultiprocessedCracker.cpp main.cpp)
target_link_libraries (SeedCrackerByNeil2.0 ${CMAKE_THREAD_LIBS_INIT})

check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
	enable_language(CUDA)
	message(STATUS "CUDA support")
	add_executable(SeedCrackerByNeil2.0_CUDA CudaCracker.cu Random.hpp Parser.cpp Utils.cpp PillarsCracker.cpp GenerationCracker.cpp generationByCubitect/generator.cpp generationByCubitect/layers.cpp StructureCracker.cpp MultiprocessedCracker.cpp main.cpp)
	target_link_libraries (SeedCrackerByNeil2.0_CUDA ${CMAKE_THREAD_LIBS_INIT})
	target_compile_definitions(SeedCrackerByNeil2.0_CUDA PRIVATE USE_CUDA)
else()
	message(STATUS "No CUDA support")
endif()
