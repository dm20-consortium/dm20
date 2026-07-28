CREATE TABLE st_face (
    face_id integer NOT NULL,
    mbr public.geometry(PolygonZ,2449),
    slope double precision,
    aspect integer,
    light_value integer,
    extended_geom_flag boolean,
    PRIMARY KEY (face_id)
);