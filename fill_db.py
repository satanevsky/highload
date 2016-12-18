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

    session.execute("""
        INSERT INTO jokes (id, text, creation_timestamp, likes, dislikes)
            VALUES(c37d661d-7e61-49ea-96a5-68c34e83db3a, 'hi, here is my joke', 111111, 10, 20) IF NOT EXISTS;
    """)




if __name__ == "__main__":
    main()
