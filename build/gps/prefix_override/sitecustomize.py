import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/exhausted/Saisarath/stuff/MRM/mrm_irc_2026/mrm_irc_2026/install/gps'
