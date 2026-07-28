CREATE TABLE st_edge (
    edge_id integer NOT NULL,
    start_node integer,
    end_node integer,
    next_left_edge integer,
    next_right_edge integer,
    left_face integer,
    right_face integer,
    geometry public.geometry(LineStringZ,2449),
    PRIMARY KEY (edge_id)
);