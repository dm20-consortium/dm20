CREATE TABLE pf_area_topo_prim (
    feat_category_num integer NOT NULL,
    feature_id integer NOT NULL,
    face_id integer NOT NULL,
    PRIMARY KEY (feat_category_num, feature_id, face_id),
    FOREIGN KEY (face_id) REFERENCES st_face(face_id),
    FOREIGN KEY (feat_category_num, feature_id) REFERENCES pf_area_feature(feat_category_num, feature_id)
);