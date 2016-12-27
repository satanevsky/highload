import sys
import random


def gen_ammo_for_get(posts_uuids, size, output_path):
    with open(output_path, 'w') as f:
        f.write("[Connection: keep-alive]\n[Host: target.example.com]\n[Cookie: None]\n")
        for _ in xrange(size):
            f.write('/hellofastcgi?id={}\n'.format(random.choice(posts_uuids)))


if __name__ == "__main__":
    with open('inserted_uuids') as f:
        posts_uuids = [el.rstrip() for el in f]
    gen_ammo_for_get(posts_uuids, int(sys.argv[1]), sys.argv[2])
