CREATE TABLE pf_line_topo_prim (
    feat_category_num integer NOT NULL,
    feature_id integer NOT NULL,
    sequence_number integer NOT NULL,
    edge_id integer NOT NULL,
    edge_orientation integer,
    start_elevation integer,
    intermitted_elevation integer,
    PRIMARY KEY (feat_category_num, feature_id, sequence_number, edge_id),
    FOREIGN KEY (edge_id) REFERENCES st_edge(edge_id),
    FOREIGN KEY (feat_category_num, feature_id) REFERENCES pf_line_feature(feat_category_num, feature_id)
);
