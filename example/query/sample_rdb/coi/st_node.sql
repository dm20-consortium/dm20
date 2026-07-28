CREATE TABLE st_node (
    node_id integer NOT NULL,
    geometry public.geometry(PointZ,2449),
    containing_face integer,
    PRIMARY KEY (node_id)
);