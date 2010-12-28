CREATE TABLE `stitchw1_licenses`.`serialnumbers` (
  `id` BIGINT(20)  NOT NULL AUTO_INCREMENT,
  `serial_number` VARCHAR(15)  NOT NULL,
  `comments` TEXT  NOT NULL,
  `timestamp` TIMESTAMP  NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
)
ENGINE = MyISAM;