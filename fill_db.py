import sys
import uuid
import string
import random
from cassandra.cluster import Cluster
from cassandra.policies import DCAwareRoundRobinPolicy


def main():
    cluster = Cluster(
        ['127.0.0.1'],
        load_balancing_policy=DCAwareRoundRobinPolicy(),
        port=9042)
    session = cluster.connect()
    session.execute('USE highload')
    session.execute('DROP TABLE IF EXISTS jokes;')
    session.execute("""
         CREATE TABLE jokes (
            id uuid PRIMARY KEY,
            text text,
            creation_timestamp timestamp,
            likes int,
            dislikes int
        )
    """)

    uuids = list()
    letters = string.ascii_uppercase + string.ascii_lowercase + string.digits
    for _ in xrange(int(sys.argv[1])):
        text = ''.join(random.choice(letters) for _ in xrange(200))
        uid = str(uuid.uuid1())
        session.execute("""
            INSERT INTO jokes (id, text, creation_timestamp, likes, dislikes)
                VALUES({}, '{}', toTimestamp(now()), 10, 20) IF NOT EXISTS;
        """.format(uid, text))
        uuids.append(uid)
    if len(sys.argv) >= 3 and sys.argv[2] == 'True':
        with open('inserted_uuids', 'w') as f:
            for el in uuids:
                f.write(el)
                f.write('\n')



if __name__ == "__main__":
    main()
