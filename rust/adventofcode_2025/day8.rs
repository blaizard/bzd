use utils::arg1_to_vector;

struct Coordinates {
    x: f64,
    y: f64,
    z: f64,
}

struct ShortestDistance {
    distance: f64,
    index: usize,
}

fn nb_junctions(index: usize, distances: &Vec<ShortestDistance>, covered: &mut Vec<bool>) -> usize {
    let mut nb = 0;

    if covered[index] == false {
        covered[index] = true;
        nb += 1;

        for index_b in 0..distances.len() {
            if distances[index_b].index == index {
                nb += nb_junctions(index_b, distances, covered);
            }
        }
        nb += nb_junctions(distances[index].index, distances, covered);
    }

    return nb;
}

fn part1(ranges: &Vec<Coordinates>) {
    let mut total = 0;
    let mut distances: Vec<ShortestDistance> = vec![];

    // Calculate the shortest distances between all boxes.
    for index_a in 0..ranges.len() {
        let mut shortest = ShortestDistance {
            distance: -1.0,
            index: 0,
        };
        let a = &ranges[index_a];
        for index_b in 0..ranges.len() {
            if index_a == index_b {
                continue;
            }
            let b = &ranges[index_b];
            let distance =
                ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z))
                    .sqrt();
            if (shortest.distance < 0.0) || (shortest.distance > distance) {
                shortest.distance = distance;
                shortest.index = index_b;
            }
        }

        println!(
            "{},{},{} with {},{},{}: {}",
            a.x,
            a.y,
            a.z,
            ranges[shortest.index].x,
            ranges[shortest.index].y,
            ranges[shortest.index].z,
            shortest.distance
        );

        distances.push(shortest);
    }

    let mut covered: Vec<bool> = vec![false; distances.len()];

    // Create the circuits.
    for index in 0..distances.len() {
        if covered[index] == true {
            continue;
        }
        let nb = nb_junctions(index, &distances, &mut covered);
        println!("{}: {}", index, nb);
    }

    println!("Part 1: {}", total);
}

fn part2(ranges: &Vec<Coordinates>) {
    let mut total = 0;

    println!("Part 2: {}", total);
}

fn main() {
    let lines = arg1_to_vector('\n', true);
    let coordinates: Vec<Coordinates> = lines
        .iter()
        .map(|line| {
            let split: Vec<&str> = line.split(',').collect();
            Coordinates {
                x: split[0].parse().unwrap(),
                y: split[1].parse().unwrap(),
                z: split[2].parse().unwrap(),
            }
        })
        .collect();

    part1(&coordinates);
    part2(&coordinates);
}
