#pragma once

std::vector<unsigned long long> gen(std::vector<Biomess> bio, unsigned long long partial, versions version);

std::vector<unsigned long long>
gen_handler(std::vector<Biomess> bio, std::vector<unsigned long long> partial, versions version);
