#pragma once

std::vector<unsigned long long> gen(const std::vector<Biomess>& bio, unsigned long long partial, versions version);

std::vector<unsigned long long>
gen_handler(const std::vector<Biomess>& bio, const std::vector<unsigned long long>& partial, versions version);
