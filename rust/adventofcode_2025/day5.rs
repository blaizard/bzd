use utils::arg1_to_vector;

fn part1(ranges: &Vec<(u64, u64)>, ids: &Vec<u64>) {
    let mut total = 0;

    for id in ids {
        for (min, max) in ranges {
            if min <= id && id <= max {
                total += 1;
                break;
            }
        }
    }

    println!("Part 1: {}", total);
}

fn part2(ranges: &Vec<(u64, u64)>) {
    let mut total = 0;

    let mut copy_ranges = ranges.to_vec();
    copy_ranges.sort();

    let mut previous_max = 0;
    for (min, max) in &mut copy_ranges {
        if *min <= previous_max {
            *min = previous_max + 1;
        }

        previous_max = previous_max.max(*max);
    }

    for (min, max) in copy_ranges {
        if min <= max {
            total += max - min + 1;
        }
    }

    println!("Part 2: {}", total);
}

fn main() {
    let lines = arg1_to_vector('\n', true);
    let index = lines.iter().position(|x| !x.contains('-')).unwrap();
    let ranges: Vec<(u64, u64)> = lines[0..index]
        .iter()
        .map(|x| {
            let (start_str, end_str) = x.split_once('-').unwrap();
            let start = start_str.parse::<u64>().unwrap();
            let end = end_str.parse::<u64>().unwrap();
            (start, end)
        })
        .collect();
    let ids: Vec<u64> = lines[index..]
        .iter()
        .map(|x| x.parse::<u64>().unwrap())
        .collect();

    part1(&ranges, &ids);
    part2(&ranges);
}
