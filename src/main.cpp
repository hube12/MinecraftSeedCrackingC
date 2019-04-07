#include "Parser.hpp"
#include "PillarsCracker.hpp"
#include "MultiprocessedCracker.hpp"
#include "GenerationCracker.hpp"

#include <fstream>
#include <wait.h>

void signalHandler(int signum) {
    std::cout << "received SIGTERM " << signum << " on group pid : " << getgid() << std::endl;
    kill(-getgid(), SIGKILL);
}

char *inputString(FILE *fp, size_t size) {
//The size is extended by the input with the value of the provisional
    char *str;
    int ch;
    size_t len = 0;
    str = (char *) realloc(nullptr, sizeof(char) * size);//size is start size
    if (!str)return str;
    while (EOF != (ch = fgetc(fp)) && ch != '\n') {
        str[len++] = (char) ch;
        if (len == size) {
            str = (char *) realloc(str, sizeof(char) * (size += 16));
            if (!str)return str;
        }
    }
    str[len++] = '\0';

    return (char *) realloc(str, sizeof(char) * len);
};

int main() {

    std::cout << "Hello, if you have any trouble just contact me on discord: NEIL#4879" << std::endl;
    std::cout << "Enter the name of your file (with the .txt): ";
    char *filename = inputString(stdin, 10);
    const Globals global_data = parse_file(filename);
    unsigned int pillar_seed = find_pillar_seed(global_data.pillars_array);
    printf("Pillar seed was found and it is: %d \n", pillar_seed);

    std::vector<int *> pipes;
    for (int i = 0; i < global_data.option.processes; i++) {
        int *fd = (int *) malloc(2 * sizeof(int));
        if (pipe(fd) == -1) {
            std::cerr << "Pipe failed" << std::endl;
            return -1;
        }
        pipes.push_back(fd);

    }
    pid_t pidMain = fork();
    if (pidMain == 0) {
        setpgid(getpid(), getppid());
        multiprocess_handler(pillar_seed, global_data.structures_array, global_data.option.processes, getppid(), pipes);
    } else {
        setpgid(getpid(), getpid());
        for (auto el:pipes) {
            close(el[1]); //close writing end
        }
        signal(SIGTERM, signalHandler);
        int status;
        int blocklist[global_data.option.processes];
        for (int i = 0; i < global_data.option.processes; i++) {
            blocklist[i] = 0;
        }
        int flag_pipe = 0;
       /* while (flag_pipe < global_data.option.processes) {
            for (unsigned i = 0; i < pipes.size(); i++) {
                if (!blocklist[i]) {

                    char progression[200];
                    int stat = read(pipes[i][0], progression, 100);
                    if (stat > 0) {
                        std::cout << progression << std::endl;
                    } else {
                        if (errno!=EAGAIN){

                            blocklist[i] = 0;
                            flag_pipe++;
                        }
                    }
                }
            }
        }*/
       while(1){
           char progression[200];
          int nread=read(pipes[0][0],progression,200);
          if (nread==-1){
              if(errno==EAGAIN){
                  sleep(1);
              }
              else{
                  printf("read error\n");
                  exit(4);
              }
          }
          else if(nread==0){
              printf("end\n");
              close(pipes[0][0]);
              break;
          }
          else{
              std::cout<<progression<<std::endl;
          }
       }

        while (waitpid(0, &status, WCONTINUED) != -1) {}


        std::vector<unsigned long long> partials_seeds = assemble_logs(global_data.option.processes);

        for (auto el:partials_seeds) {
            std::cout << "Potential seed " << el << std::endl;
        }
        if (partials_seeds.size() > 1) {
            std::cout << "There is too much seed, let's try to reduce that number\n";
        }
        std::vector<unsigned long long> final_seeds = gen_handler(global_data.biome, partials_seeds,
                                                                  global_data.option.version);
        std::ofstream save("final_seeds.txt", std::ios_base::out | std::ios::trunc);
        if (save.is_open()) {
            for (auto el:final_seeds) {
                std::cout << "Final seeds " << (int64_t) el << std::endl;
                save << (int64_t) el << std::endl;
            }
        } else {
            std::cout << "save file was not loaded" << std::endl;
            throw std::runtime_error("save file was not loaded");
        }
        if (!final_seeds.empty()) {
            std::cout << "Well done, you have your seed here in the chat and also in final_seeds.txt, enjoy!"
                      << std::endl;
            std::cout << "You can still contact me on discord: Neil#4879 if you have any issue!" << std::endl;
            while (true) {
                std::cout << "Quit? (Y/N)" << std::endl;
                char *output = inputString(stdin, 10);
                if (output[0] == 'Y' || output[0] == 'y') {
                    return 0;
                }
            }
        } else {
            std::cout
                    << "There is a huge error, either its you or its coz you are on spigot, pls send me your data.txt on discord (NEIL#4879) and "
                       "i will try to help you get the seed, join any log with it" << std::endl;
            if (!partials_seeds.empty()) {
                for (auto el:partials_seeds) {
                    std::cout << el << std::endl;
                }
                std::cout << pillar_seed << std::endl;
            }
            while (true) {
                std::cout << "Quit? (Y/N)" << std::endl;
                char *output = inputString(stdin, 10);
                if (output[0] == 'Y' || output[0] == 'y') {
                    return 0;
                }
            }
        }

    }
}
