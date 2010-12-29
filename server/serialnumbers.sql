CREATE TABLE `stitchw1_licenses`.`serialnumbers` (
  `id` BIGINT(20)  NOT NULL AUTO_INCREMENT,
  `serial_number` VARCHAR(15)  NOT NULL,
  `comments` TEXT  NOT NULL,
  `ip_address` VARCHAR(15)  NOT NULL,
  `timestamp` TIMESTAMP  NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
)
ENGINE = MyISAM;


CREATE TABLE `stitchw1_licenses`.`licenses` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `serial_number` varchar(15) NOT NULL,
  `license` varchar(20) NOT NULL,
  `email` varchar(100) NOT NULL,
  `first_name` varchar(100) NOT NULL,
  `last_name` varchar(100) NOT NULL,
  `ip_address` varchar(15) NOT NULL,

  PRIMARY KEY (`id`)
) ENGINE=MyISAM;