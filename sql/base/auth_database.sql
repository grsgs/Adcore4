/*
Navicat MySQL Data Transfer

Source Server         : Project
Source Server Version : 50509
Source Host           : localhost:3306
Source Database       : auth

Target Server Type    : MYSQL
Target Server Version : 50509
File Encoding         : 65001

Date: 2013-07-20 07:25:11
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `account`
-- ----------------------------
DROP TABLE IF EXISTS `account`;
CREATE TABLE `account` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `username` varchar(32) NOT NULL DEFAULT '',
  `sha_pass_hash` varchar(40) NOT NULL DEFAULT '',
  `sessionkey` varchar(80) NOT NULL DEFAULT '',
  `v` varchar(64) NOT NULL DEFAULT '',
  `s` varchar(64) NOT NULL DEFAULT '',
  `email` varchar(254) NOT NULL DEFAULT '',
  `joindate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `last_ip` varchar(15) NOT NULL DEFAULT '127.0.0.1',
  `failed_logins` int(10) unsigned NOT NULL DEFAULT '0',
  `locked` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `lock_country` varchar(2) NOT NULL DEFAULT '00',
  `last_login` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `online` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `expansion` tinyint(3) unsigned NOT NULL DEFAULT '3',
  `mutetime` bigint(20) NOT NULL DEFAULT '0',
  `mutereason` varchar(255) NOT NULL DEFAULT '',
  `muteby` varchar(50) NOT NULL DEFAULT '',
  `locale` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `os` varchar(3) NOT NULL DEFAULT '',
  `recruiter` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_username` (`username`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COMMENT='Account System';

-- ----------------------------
-- Records of account
-- ----------------------------
INSERT INTO account VALUES ('1', 'ADMIN', '8301316D0D8448A34FA6D0C6BF1CBFA2B4A1A93A', '', '', '', '', '2013-07-19 21:56:36', '127.0.0.1', '0', '0', '00', '0000-00-00 00:00:00', '0', '3', '0', '', '', '0', '', '0');

-- ----------------------------
-- Table structure for `account_access`
-- ----------------------------
DROP TABLE IF EXISTS `account_access`;
CREATE TABLE `account_access` (
  `id` int(10) unsigned NOT NULL,
  `gmlevel` tinyint(3) unsigned NOT NULL,
  `RealmID` int(11) NOT NULL DEFAULT '-1',
  PRIMARY KEY (`id`,`RealmID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of account_access
-- ----------------------------

-- ----------------------------
-- Table structure for `account_banned`
-- ----------------------------
DROP TABLE IF EXISTS `account_banned`;
CREATE TABLE `account_banned` (
  `id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Account id',
  `bandate` int(10) unsigned NOT NULL DEFAULT '0',
  `unbandate` int(10) unsigned NOT NULL DEFAULT '0',
  `bannedby` varchar(50) NOT NULL,
  `banreason` varchar(255) NOT NULL,
  `active` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`,`bandate`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Ban List';

-- ----------------------------
-- Records of account_banned
-- ----------------------------

-- ----------------------------
-- Table structure for `autobroadcast`
-- ----------------------------
DROP TABLE IF EXISTS `autobroadcast`;
CREATE TABLE `autobroadcast` (
  `realmid` int(11) NOT NULL DEFAULT '-1',
  `id` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `weight` tinyint(3) unsigned DEFAULT '1',
  `text` longtext NOT NULL,
  PRIMARY KEY (`id`,`realmid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of autobroadcast
-- ----------------------------

-- ----------------------------
-- Table structure for `ip2nation`
-- ----------------------------
DROP TABLE IF EXISTS `ip2nation`;
CREATE TABLE `ip2nation` (
  `ip` int(11) unsigned NOT NULL DEFAULT '0',
  `country` char(2) NOT NULL DEFAULT '',
  KEY `ip` (`ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of ip2nation
-- ----------------------------

-- ----------------------------
-- Table structure for `ip2nationcountries`
-- ----------------------------
DROP TABLE IF EXISTS `ip2nationcountries`;
CREATE TABLE `ip2nationcountries` (
  `code` varchar(4) NOT NULL DEFAULT '',
  `iso_code_2` varchar(2) NOT NULL DEFAULT '',
  `iso_code_3` varchar(3) DEFAULT '',
  `iso_country` varchar(255) NOT NULL DEFAULT '',
  `country` varchar(255) NOT NULL DEFAULT '',
  `lat` float NOT NULL DEFAULT '0',
  `lon` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`code`),
  KEY `code` (`code`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of ip2nationcountries
-- ----------------------------

-- ----------------------------
-- Table structure for `ip_banned`
-- ----------------------------
DROP TABLE IF EXISTS `ip_banned`;
CREATE TABLE `ip_banned` (
  `ip` varchar(15) NOT NULL DEFAULT '127.0.0.1',
  `bandate` int(10) unsigned NOT NULL,
  `unbandate` int(10) unsigned NOT NULL,
  `bannedby` varchar(50) NOT NULL DEFAULT '[Console]',
  `banreason` varchar(255) NOT NULL DEFAULT 'no reason',
  PRIMARY KEY (`ip`,`bandate`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Banned IPs';

-- ----------------------------
-- Records of ip_banned
-- ----------------------------

-- ----------------------------
-- Table structure for `logs`
-- ----------------------------
DROP TABLE IF EXISTS `logs`;
CREATE TABLE `logs` (
  `time` int(10) unsigned NOT NULL,
  `realm` int(10) unsigned NOT NULL,
  `type` tinyint(3) unsigned NOT NULL,
  `level` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `string` text CHARACTER SET latin1
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of logs
-- ----------------------------

-- ----------------------------
-- Table structure for `rbac_account_groups`
-- ----------------------------
DROP TABLE IF EXISTS `rbac_account_groups`;
CREATE TABLE `rbac_account_groups` (
  `accountId` int(10) unsigned NOT NULL COMMENT 'Account id',
  `groupId` int(10) unsigned NOT NULL COMMENT 'Group id',
  `realmId` int(11) NOT NULL DEFAULT '-1' COMMENT 'Realm Id, -1 means all',
  PRIMARY KEY (`accountId`,`groupId`,`realmId`),
  KEY `fk__rbac_account_groups__rbac_groups` (`groupId`),
  CONSTRAINT `fk__rbac_account_groups__account` FOREIGN KEY (`accountId`) REFERENCES `account` (`id`) ON DELETE CASCADE,
  CONSTRAINT `fk__rbac_account_groups__rbac_groups` FOREIGN KEY (`groupId`) REFERENCES `rbac_groups` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Account-Group relation';

-- ----------------------------
-- Records of rbac_account_groups
-- ----------------------------
INSERT INTO rbac_account_groups VALUES ('1', '1', '-1');

-- ----------------------------
-- Table structure for `rbac_account_permissions`
-- ----------------------------
DROP TABLE IF EXISTS `rbac_account_permissions`;
CREATE TABLE `rbac_account_permissions` (
  `accountId` int(10) unsigned NOT NULL COMMENT 'Account id',
  `permissionId` int(10) unsigned NOT NULL COMMENT 'Permission id',
  `granted` tinyint(1) NOT NULL DEFAULT '1' COMMENT 'Granted = 1, Denied = 0',
  `realmId` int(11) NOT NULL DEFAULT '-1' COMMENT 'Realm Id, -1 means all',
  PRIMARY KEY (`accountId`,`permissionId`,`realmId`),
  KEY `fk__rbac_account_roles__rbac_permissions` (`permissionId`),
  CONSTRAINT `fk__rbac_account_permissions__account` FOREIGN KEY (`accountId`) REFERENCES `account` (`id`) ON DELETE CASCADE,
  CONSTRAINT `fk__rbac_account_roles__rbac_permissions` FOREIGN KEY (`permissionId`) REFERENCES `rbac_permissions` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Account-Permission relation';

-- ----------------------------
-- Records of rbac_account_permissions
-- ----------------------------

-- ----------------------------
-- Table structure for `rbac_account_roles`
-- ----------------------------
DROP TABLE IF EXISTS `rbac_account_roles`;
CREATE TABLE `rbac_account_roles` (
  `accountId` int(10) unsigned NOT NULL COMMENT 'Account id',
  `roleId` int(10) unsigned NOT NULL COMMENT 'Role id',
  `granted` tinyint(1) NOT NULL DEFAULT '1' COMMENT 'Granted = 1, Denied = 0',
  `realmId` int(11) NOT NULL DEFAULT '-1' COMMENT 'Realm Id, -1 means all',
  PRIMARY KEY (`accountId`,`roleId`,`realmId`),
  KEY `fk__rbac_account_roles__rbac_roles` (`roleId`),
  CONSTRAINT `fk__rbac_account_roles__account` FOREIGN KEY (`accountId`) REFERENCES `account` (`id`) ON DELETE CASCADE,
  CONSTRAINT `fk__rbac_account_roles__rbac_roles` FOREIGN KEY (`roleId`) REFERENCES `rbac_roles` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Account-Role relation';

-- ----------------------------
-- Records of rbac_account_roles
-- ----------------------------

-- ----------------------------
-- Table structure for `rbac_groups`
-- ----------------------------
DROP TABLE IF EXISTS `rbac_groups`;
CREATE TABLE `rbac_groups` (
  `id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Group id',
  `name` varchar(100) NOT NULL COMMENT 'Group name',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Group List';

-- ----------------------------
-- Records of rbac_groups
-- ----------------------------
INSERT INTO rbac_groups VALUES ('1', 'Player');
INSERT INTO rbac_groups VALUES ('2', 'Moderator');
INSERT INTO rbac_groups VALUES ('3', 'GameMaster');
INSERT INTO rbac_groups VALUES ('4', 'Administrator');

-- ----------------------------
-- Table structure for `rbac_group_roles`
-- ----------------------------
DROP TABLE IF EXISTS `rbac_group_roles`;
CREATE TABLE `rbac_group_roles` (
  `groupId` int(10) unsigned NOT NULL COMMENT 'group id',
  `roleId` int(10) unsigned NOT NULL COMMENT 'Role id',
  PRIMARY KEY (`groupId`,`roleId`),
  KEY `fk__rbac_group_roles__rbac_roles` (`roleId`),
  CONSTRAINT `fk__rbac_group_roles__rbac_groups` FOREIGN KEY (`groupId`) REFERENCES `rbac_groups` (`id`) ON DELETE CASCADE,
  CONSTRAINT `fk__rbac_group_roles__rbac_roles` FOREIGN KEY (`roleId`) REFERENCES `rbac_roles` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Group Role relation';

-- ----------------------------
-- Records of rbac_group_roles
-- ----------------------------
INSERT INTO rbac_group_roles VALUES ('1', '1');
INSERT INTO rbac_group_roles VALUES ('2', '2');
INSERT INTO rbac_group_roles VALUES ('3', '3');
INSERT INTO rbac_group_roles VALUES ('4', '4');
INSERT INTO rbac_group_roles VALUES ('2', '5');
INSERT INTO rbac_group_roles VALUES ('1', '6');
INSERT INTO rbac_group_roles VALUES ('1', '7');
INSERT INTO rbac_group_roles VALUES ('2', '8');
INSERT INTO rbac_group_roles VALUES ('3', '8');
INSERT INTO rbac_group_roles VALUES ('4', '8');
INSERT INTO rbac_group_roles VALUES ('2', '9');
INSERT INTO rbac_group_roles VALUES ('3', '9');
INSERT INTO rbac_group_roles VALUES ('4', '9');
INSERT INTO rbac_group_roles VALUES ('2', '10');
INSERT INTO rbac_group_roles VALUES ('3', '10');
INSERT INTO rbac_group_roles VALUES ('4', '10');
INSERT INTO rbac_group_roles VALUES ('2', '11');
INSERT INTO rbac_group_roles VALUES ('3', '11');
INSERT INTO rbac_group_roles VALUES ('4', '11');
INSERT INTO rbac_group_roles VALUES ('2', '12');
INSERT INTO rbac_group_roles VALUES ('3', '12');
INSERT INTO rbac_group_roles VALUES ('4', '12');
INSERT INTO rbac_group_roles VALUES ('2', '13');
INSERT INTO rbac_group_roles VALUES ('3', '13');
INSERT INTO rbac_group_roles VALUES ('4', '13');
INSERT INTO rbac_group_roles VALUES ('2', '14');
INSERT INTO rbac_group_roles VALUES ('3', '14');
INSERT INTO rbac_group_roles VALUES ('4', '14');
INSERT INTO rbac_group_roles VALUES ('2', '15');
INSERT INTO rbac_group_roles VALUES ('3', '15');
INSERT INTO rbac_group_roles VALUES ('4', '15');
INSERT INTO rbac_group_roles VALUES ('2', '16');
INSERT INTO rbac_group_roles VALUES ('3', '16');
INSERT INTO rbac_group_roles VALUES ('4', '16');
INSERT INTO rbac_group_roles VALUES ('2', '17');
INSERT INTO rbac_group_roles VALUES ('3', '17');
INSERT INTO rbac_group_roles VALUES ('4', '17');
INSERT INTO rbac_group_roles VALUES ('4', '18');
INSERT INTO rbac_group_roles VALUES ('2', '19');
INSERT INTO rbac_group_roles VALUES ('3', '19');
INSERT INTO rbac_group_roles VALUES ('4', '19');
INSERT INTO rbac_group_roles VALUES ('2', '20');
INSERT INTO rbac_group_roles VALUES ('3', '20');
INSERT INTO rbac_group_roles VALUES ('4', '20');
INSERT INTO rbac_group_roles VALUES ('2', '21');
INSERT INTO rbac_group_roles VALUES ('3', '21');
INSERT INTO rbac_group_roles VALUES ('4', '21');
INSERT INTO rbac_group_roles VALUES ('2', '22');
INSERT INTO rbac_group_roles VALUES ('3', '22');
INSERT INTO rbac_group_roles VALUES ('4', '22');
INSERT INTO rbac_group_roles VALUES ('4', '23');
INSERT INTO rbac_group_roles VALUES ('2', '24');
INSERT INTO rbac_group_roles VALUES ('3', '24');
INSERT INTO rbac_group_roles VALUES ('4', '24');
INSERT INTO rbac_group_roles VALUES ('2', '25');
INSERT INTO rbac_group_roles VALUES ('3', '25');
INSERT INTO rbac_group_roles VALUES ('4', '25');
INSERT INTO rbac_group_roles VALUES ('2', '26');
INSERT INTO rbac_group_roles VALUES ('3', '26');
INSERT INTO rbac_group_roles VALUES ('4', '26');
INSERT INTO rbac_group_roles VALUES ('2', '27');
INSERT INTO rbac_group_roles VALUES ('3', '27');
INSERT INTO rbac_group_roles VALUES ('4', '27');
INSERT INTO rbac_group_roles VALUES ('2', '28');
INSERT INTO rbac_group_roles VALUES ('3', '28');
INSERT INTO rbac_group_roles VALUES ('4', '28');
INSERT INTO rbac_group_roles VALUES ('2', '29');
INSERT INTO rbac_group_roles VALUES ('3', '29');
INSERT INTO rbac_group_roles VALUES ('4', '29');
INSERT INTO rbac_group_roles VALUES ('2', '30');
INSERT INTO rbac_group_roles VALUES ('3', '30');
INSERT INTO rbac_group_roles VALUES ('4', '30');
INSERT INTO rbac_group_roles VALUES ('2', '32');
INSERT INTO rbac_group_roles VALUES ('3', '32');
INSERT INTO rbac_group_roles VALUES ('4', '32');
INSERT INTO rbac_group_roles VALUES ('2', '33');
INSERT INTO rbac_group_roles VALUES ('3', '33');
INSERT INTO rbac_group_roles VALUES ('4', '33');
INSERT INTO rbac_group_roles VALUES ('1', '34');
INSERT INTO rbac_group_roles VALUES ('2', '35');
INSERT INTO rbac_group_roles VALUES ('3', '35');
INSERT INTO rbac_group_roles VALUES ('4', '35');
INSERT INTO rbac_group_roles VALUES ('2', '36');
INSERT INTO rbac_group_roles VALUES ('3', '36');
INSERT INTO rbac_group_roles VALUES ('4', '36');
INSERT INTO rbac_group_roles VALUES ('2', '37');
INSERT INTO rbac_group_roles VALUES ('3', '37');
INSERT INTO rbac_group_roles VALUES ('4', '37');
INSERT INTO rbac_group_roles VALUES ('2', '38');
INSERT INTO rbac_group_roles VALUES ('3', '38');
INSERT INTO rbac_group_roles VALUES ('4', '38');
INSERT INTO rbac_group_roles VALUES ('3', '39');
INSERT INTO rbac_group_roles VALUES ('4', '39');

-- ----------------------------
-- Table structure for `rbac_permissions`
-- ----------------------------
DROP TABLE IF EXISTS `rbac_permissions`;
CREATE TABLE `rbac_permissions` (
  `id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Permission id',
  `name` varchar(100) NOT NULL COMMENT 'Permission name',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Permission List';

-- ----------------------------
-- Records of rbac_permissions
-- ----------------------------
INSERT INTO rbac_permissions VALUES ('1', 'Instant logout');
INSERT INTO rbac_permissions VALUES ('2', 'Skip Queue');
INSERT INTO rbac_permissions VALUES ('3', 'Join Normal Battleground');
INSERT INTO rbac_permissions VALUES ('4', 'Join Random Battleground');
INSERT INTO rbac_permissions VALUES ('5', 'Join Arenas');
INSERT INTO rbac_permissions VALUES ('6', 'Join Dungeon Finder');
INSERT INTO rbac_permissions VALUES ('7', 'Player Commands (Temporal till commands moved to rbac)');
INSERT INTO rbac_permissions VALUES ('8', 'Moderator Commands (Temporal till commands moved to rbac)');
INSERT INTO rbac_permissions VALUES ('9', 'GameMaster Commands (Temporal till commands moved to rbac)');
INSERT INTO rbac_permissions VALUES ('10', 'Administrator Commands (Temporal till commands moved to rbac)');
INSERT INTO rbac_permissions VALUES ('11', 'Log GM trades');
INSERT INTO rbac_permissions VALUES ('13', 'Skip Instance required bosses check');
INSERT INTO rbac_permissions VALUES ('14', 'Skip character creation team mask check');
INSERT INTO rbac_permissions VALUES ('15', 'Skip character creation class mask check');
INSERT INTO rbac_permissions VALUES ('16', 'Skip character creation race mask check');
INSERT INTO rbac_permissions VALUES ('17', 'Skip character creation reserved name check');
INSERT INTO rbac_permissions VALUES ('18', 'Skip character creation heroic min level check');
INSERT INTO rbac_permissions VALUES ('19', 'Skip needed requirements to use channel check');
INSERT INTO rbac_permissions VALUES ('20', 'Skip disable map check');
INSERT INTO rbac_permissions VALUES ('21', 'Skip reset talents when used more than allowed check');
INSERT INTO rbac_permissions VALUES ('22', 'Skip spam chat check');
INSERT INTO rbac_permissions VALUES ('23', 'Skip over-speed ping check');
INSERT INTO rbac_permissions VALUES ('24', 'Two side faction characters on the same account');
INSERT INTO rbac_permissions VALUES ('25', 'Allow say chat between factions');
INSERT INTO rbac_permissions VALUES ('26', 'Allow channel chat between factions');
INSERT INTO rbac_permissions VALUES ('27', 'Two side mail interaction');
INSERT INTO rbac_permissions VALUES ('28', 'See two side who list');
INSERT INTO rbac_permissions VALUES ('29', 'Add friends of other faction');
INSERT INTO rbac_permissions VALUES ('30', 'Save character without delay with .save command');
INSERT INTO rbac_permissions VALUES ('31', 'Use params with .unstuck command');
INSERT INTO rbac_permissions VALUES ('32', 'Can be assigned tickets with .assign ticket command');
INSERT INTO rbac_permissions VALUES ('33', 'Notify if a command was not found');
INSERT INTO rbac_permissions VALUES ('34', 'Check if should appear in list using .gm ingame command');
INSERT INTO rbac_permissions VALUES ('35', 'See all security levels with who command');
INSERT INTO rbac_permissions VALUES ('36', 'Filter whispers');
INSERT INTO rbac_permissions VALUES ('37', 'Use staff badge in chat');
INSERT INTO rbac_permissions VALUES ('38', 'Resurrect with full Health Points');
INSERT INTO rbac_permissions VALUES ('39', 'Restore saved gm setting states');
INSERT INTO rbac_permissions VALUES ('40', 'Allows to add a gm to friend list');
INSERT INTO rbac_permissions VALUES ('41', 'Use Config option START_GM_LEVEL to assign new character level');
INSERT INTO rbac_permissions VALUES ('42', 'Allows to use CMSG_WORLD_TELEPORT opcode');
INSERT INTO rbac_permissions VALUES ('43', 'Allows to use CMSG_WHOIS opcode');
INSERT INTO rbac_permissions VALUES ('44', 'Receive global GM messages/texts');
INSERT INTO rbac_permissions VALUES ('45', 'Join channels without announce');
INSERT INTO rbac_permissions VALUES ('46', 'Change channel settings without being channel moderator');
INSERT INTO rbac_permissions VALUES ('47', 'Enables lower security than target check');
INSERT INTO rbac_permissions VALUES ('48', 'Enable IP, Last Login and EMail output in pinfo');

-- ----------------------------
-- Table structure for `rbac_roles`
-- ----------------------------
DROP TABLE IF EXISTS `rbac_roles`;
CREATE TABLE `rbac_roles` (
  `id` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Role id',
  `name` varchar(100) NOT NULL COMMENT 'Role name',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Roles List';

-- ----------------------------
-- Records of rbac_roles
-- ----------------------------
INSERT INTO rbac_roles VALUES ('1', 'Player Commands');
INSERT INTO rbac_roles VALUES ('2', 'Moderator Commands');
INSERT INTO rbac_roles VALUES ('3', 'GameMaster Commands');
INSERT INTO rbac_roles VALUES ('4', 'Administrator Commands');
INSERT INTO rbac_roles VALUES ('5', 'Quick Login/Logout');
INSERT INTO rbac_roles VALUES ('6', 'Use Battleground/Arenas');
INSERT INTO rbac_roles VALUES ('7', 'Use Dungeon Finder');
INSERT INTO rbac_roles VALUES ('8', 'Log GM trades');
INSERT INTO rbac_roles VALUES ('9', 'Skip Instance required bosses check');
INSERT INTO rbac_roles VALUES ('10', 'Ticket management');
INSERT INTO rbac_roles VALUES ('11', 'Instant .save');
INSERT INTO rbac_roles VALUES ('12', 'Allow params with .unstuck');
INSERT INTO rbac_roles VALUES ('13', 'Full HP after resurrect');
INSERT INTO rbac_roles VALUES ('14', 'Appear in GM ingame list');
INSERT INTO rbac_roles VALUES ('15', 'Use staff badge in chat');
INSERT INTO rbac_roles VALUES ('16', 'Receive global GM messages/texts');
INSERT INTO rbac_roles VALUES ('17', 'Skip over-speed ping check');
INSERT INTO rbac_roles VALUES ('18', 'Allows Admin Opcodes');
INSERT INTO rbac_roles VALUES ('19', 'Two side mail interaction');
INSERT INTO rbac_roles VALUES ('20', 'Notify if a command was not found');
INSERT INTO rbac_roles VALUES ('21', 'Enables lower security than target check');
INSERT INTO rbac_roles VALUES ('22', 'Skip disable map check');
INSERT INTO rbac_roles VALUES ('23', 'Skip reset talents when used more than allowed check');
INSERT INTO rbac_roles VALUES ('24', 'Skip spam chat check');
INSERT INTO rbac_roles VALUES ('25', 'Restore saved gm setting states');
INSERT INTO rbac_roles VALUES ('26', 'Use Config option START_GM_LEVEL to assign new character level');
INSERT INTO rbac_roles VALUES ('27', 'Skip needed requirements to use channel check');
INSERT INTO rbac_roles VALUES ('28', 'Allow say chat between factions');
INSERT INTO rbac_roles VALUES ('29', 'Filter whispers');
INSERT INTO rbac_roles VALUES ('30', 'Allow channel chat between factions');
INSERT INTO rbac_roles VALUES ('31', 'Join channels without announce');
INSERT INTO rbac_roles VALUES ('32', 'Change channel settings without being channel moderator');
INSERT INTO rbac_roles VALUES ('33', 'Skip character creation checks');
INSERT INTO rbac_roles VALUES ('34', 'Two side faction characters on the same account');
INSERT INTO rbac_roles VALUES ('35', 'See two side who list');
INSERT INTO rbac_roles VALUES ('36', 'Add friends of other faction');
INSERT INTO rbac_roles VALUES ('37', 'See all security levels with who command');
INSERT INTO rbac_roles VALUES ('38', 'Allows to add a gm to friend list');
INSERT INTO rbac_roles VALUES ('39', 'Enable IP, Last Login and EMail output in pinfo');

-- ----------------------------
-- Table structure for `rbac_role_permissions`
-- ----------------------------
DROP TABLE IF EXISTS `rbac_role_permissions`;
CREATE TABLE `rbac_role_permissions` (
  `roleId` int(10) unsigned NOT NULL COMMENT 'Role id',
  `permissionId` int(10) unsigned NOT NULL COMMENT 'Permission id',
  PRIMARY KEY (`roleId`,`permissionId`),
  KEY `fk__role_permissions__rbac_permissions` (`permissionId`),
  CONSTRAINT `fk__role_permissions__rbac_permissions` FOREIGN KEY (`permissionId`) REFERENCES `rbac_permissions` (`id`) ON DELETE CASCADE,
  CONSTRAINT `fk__role_permissions__rbac_roles` FOREIGN KEY (`roleId`) REFERENCES `rbac_roles` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Role Permission relation';

-- ----------------------------
-- Records of rbac_role_permissions
-- ----------------------------
INSERT INTO rbac_role_permissions VALUES ('5', '1');
INSERT INTO rbac_role_permissions VALUES ('5', '2');
INSERT INTO rbac_role_permissions VALUES ('6', '3');
INSERT INTO rbac_role_permissions VALUES ('6', '4');
INSERT INTO rbac_role_permissions VALUES ('6', '5');
INSERT INTO rbac_role_permissions VALUES ('7', '6');
INSERT INTO rbac_role_permissions VALUES ('1', '7');
INSERT INTO rbac_role_permissions VALUES ('2', '8');
INSERT INTO rbac_role_permissions VALUES ('3', '9');
INSERT INTO rbac_role_permissions VALUES ('4', '10');
INSERT INTO rbac_role_permissions VALUES ('8', '11');
INSERT INTO rbac_role_permissions VALUES ('9', '13');
INSERT INTO rbac_role_permissions VALUES ('33', '14');
INSERT INTO rbac_role_permissions VALUES ('33', '15');
INSERT INTO rbac_role_permissions VALUES ('33', '16');
INSERT INTO rbac_role_permissions VALUES ('33', '17');
INSERT INTO rbac_role_permissions VALUES ('33', '18');
INSERT INTO rbac_role_permissions VALUES ('27', '19');
INSERT INTO rbac_role_permissions VALUES ('22', '20');
INSERT INTO rbac_role_permissions VALUES ('23', '21');
INSERT INTO rbac_role_permissions VALUES ('24', '22');
INSERT INTO rbac_role_permissions VALUES ('17', '23');
INSERT INTO rbac_role_permissions VALUES ('34', '24');
INSERT INTO rbac_role_permissions VALUES ('28', '25');
INSERT INTO rbac_role_permissions VALUES ('30', '26');
INSERT INTO rbac_role_permissions VALUES ('19', '27');
INSERT INTO rbac_role_permissions VALUES ('35', '28');
INSERT INTO rbac_role_permissions VALUES ('36', '29');
INSERT INTO rbac_role_permissions VALUES ('11', '30');
INSERT INTO rbac_role_permissions VALUES ('12', '31');
INSERT INTO rbac_role_permissions VALUES ('10', '32');
INSERT INTO rbac_role_permissions VALUES ('20', '33');
INSERT INTO rbac_role_permissions VALUES ('14', '34');
INSERT INTO rbac_role_permissions VALUES ('37', '35');
INSERT INTO rbac_role_permissions VALUES ('29', '36');
INSERT INTO rbac_role_permissions VALUES ('15', '37');
INSERT INTO rbac_role_permissions VALUES ('13', '38');
INSERT INTO rbac_role_permissions VALUES ('25', '39');
INSERT INTO rbac_role_permissions VALUES ('38', '40');
INSERT INTO rbac_role_permissions VALUES ('26', '41');
INSERT INTO rbac_role_permissions VALUES ('18', '42');
INSERT INTO rbac_role_permissions VALUES ('18', '43');
INSERT INTO rbac_role_permissions VALUES ('16', '44');
INSERT INTO rbac_role_permissions VALUES ('31', '45');
INSERT INTO rbac_role_permissions VALUES ('32', '46');
INSERT INTO rbac_role_permissions VALUES ('21', '47');
INSERT INTO rbac_role_permissions VALUES ('39', '48');

-- ----------------------------
-- Table structure for `rbac_security_level_groups`
-- ----------------------------
DROP TABLE IF EXISTS `rbac_security_level_groups`;
CREATE TABLE `rbac_security_level_groups` (
  `secId` int(10) unsigned NOT NULL COMMENT 'Security Level id',
  `groupId` int(10) unsigned NOT NULL COMMENT 'group id',
  PRIMARY KEY (`secId`,`groupId`),
  KEY `fk__rbac_security_level_groups__rbac_groups` (`groupId`),
  CONSTRAINT `fk__rbac_security_level_groups__rbac_groups` FOREIGN KEY (`groupId`) REFERENCES `rbac_groups` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Default groups to assign when an account is set gm level';

-- ----------------------------
-- Records of rbac_security_level_groups
-- ----------------------------
INSERT INTO rbac_security_level_groups VALUES ('0', '1');
INSERT INTO rbac_security_level_groups VALUES ('1', '1');
INSERT INTO rbac_security_level_groups VALUES ('2', '1');
INSERT INTO rbac_security_level_groups VALUES ('3', '1');
INSERT INTO rbac_security_level_groups VALUES ('1', '2');
INSERT INTO rbac_security_level_groups VALUES ('2', '2');
INSERT INTO rbac_security_level_groups VALUES ('3', '2');
INSERT INTO rbac_security_level_groups VALUES ('2', '3');
INSERT INTO rbac_security_level_groups VALUES ('3', '3');
INSERT INTO rbac_security_level_groups VALUES ('3', '4');

-- ----------------------------
-- Table structure for `realmcharacters`
-- ----------------------------
DROP TABLE IF EXISTS `realmcharacters`;
CREATE TABLE `realmcharacters` (
  `realmid` int(10) unsigned NOT NULL DEFAULT '0',
  `acctid` int(10) unsigned NOT NULL,
  `numchars` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`realmid`,`acctid`),
  KEY `acctid` (`acctid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Realm Character Tracker';

-- ----------------------------
-- Records of realmcharacters
-- ----------------------------
INSERT INTO realmcharacters VALUES ('1', '1', '0');

-- ----------------------------
-- Table structure for `realmlist`
-- ----------------------------
DROP TABLE IF EXISTS `realmlist`;
CREATE TABLE `realmlist` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(32) NOT NULL DEFAULT '',
  `address` varchar(255) NOT NULL DEFAULT '127.0.0.1',
  `localAddress` varchar(255) NOT NULL DEFAULT '127.0.0.1',
  `localSubnetMask` varchar(255) NOT NULL DEFAULT '255.255.255.0',
  `port` smallint(5) unsigned NOT NULL DEFAULT '8085',
  `icon` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `flag` tinyint(3) unsigned NOT NULL DEFAULT '2',
  `timezone` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `allowedSecurityLevel` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `population` float unsigned NOT NULL DEFAULT '0',
  `gamebuild` int(10) unsigned NOT NULL DEFAULT '15595',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COMMENT='Realm System';

-- ----------------------------
-- Records of realmlist
-- ----------------------------
INSERT INTO realmlist VALUES ('1', 'Mist of Depths', '127.0.0.1', '127.0.0.1', '255.255.255.0', '8085', '0', '0', '1', '0', '0', '15595');

-- ----------------------------
-- Table structure for `uptime`
-- ----------------------------
DROP TABLE IF EXISTS `uptime`;
CREATE TABLE `uptime` (
  `realmid` int(10) unsigned NOT NULL,
  `starttime` int(10) unsigned NOT NULL DEFAULT '0',
  `uptime` int(10) unsigned NOT NULL DEFAULT '0',
  `maxplayers` smallint(5) unsigned NOT NULL DEFAULT '0',
  `revision` varchar(255) NOT NULL DEFAULT 'Trinitycore',
  PRIMARY KEY (`realmid`,`starttime`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Uptime system';

-- ----------------------------
-- Records of uptime
-- ----------------------------
INSERT INTO uptime VALUES ('1', '1374263721', '0', '0', 'ArkCORE rev. unknown 1970-01-01 00:00:00 +0000 (Archived branch) (Win64, Release)');
INSERT INTO uptime VALUES ('1', '1374263749', '0', '0', 'ArkCORE rev. unknown 1970-01-01 00:00:00 +0000 (Archived branch) (Win64, Release)');
