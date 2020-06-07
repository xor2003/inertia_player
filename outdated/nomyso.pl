#!/usr/bin/perl
#
# NOMYSO MASM/TASM to NASM source converter
# Program released under the OSI-approved Artistic License.
# Read the file LICENSE.TXT included with this package for details.
# Copyright 2005-2009 Michael Devore
#
# Source website: www.devoresoftware.com/nomyso
# Please post questions and comments on the I Fix Scripts blog at http://ifixscripts.wordpress.com
#
# Hi to Judy. Always.
#
# Begun: September 2005
# Version 1.0 - Released early November 2005
# Version 2.0 - Released late November 2005
# Version 3.0 - Released December 2005
# Version 4.0 - Released May 2008
# Version 4.3 - Current, released October 2009

use strict;
use warnings;

# globals

# constants
use constant UNKNOWN			=> 0;
use constant EXTRAOPS		=>	0b0000000100000000;
use constant REMOVED			=> 0b0000001000000000;
use constant TRANSLATED		=> 0b0000010000000000;
use constant PROCESSED		=> 0b0000100000000000;
use constant MULTILINE		=> 0b0001000000000000;
use constant COMMENTED		=> 0b0010000000000000;
use constant IGNORED			=> 0b0100000000000000;
#use constant UNKNOWN			=> 0b1000000000000000;

# static control variables
my $version = "4.3";
my $copyright_years = "2005-2009";
my $project = "MASM/TASM to NASM source converter";

# static strings
my $fatal_message = "\nFatal error: ";
my $stopped_message = "\nProgram stopped.\n";
my @registers = qw(
	AL	AH	AX	EAX
	BL	BH	BX	EBX
	CL	CH	CX	ECX
	DL	DH	DX	EDX
	DI	EDI
	SI	ESI
	BP	EBP
	SP ESP
	CS DS ES FS GS SS
	CR0 CR2 CR3 CR4
	DR0 DR1 DR2 DR3 DR4 DR5 DR6 DR7
	);
# single-quote, don't substitute
my $valid_symbol_pattern = '[A-Za-z_\\@\\$\\?][A-Za-z_\\@0-9\\$\\?]*';
my $valid_symbol_no_restrictions = '[A-Za-z_\\@0-9\\$\\?]+';
my $valid_symbol_no_local = '[A-Za-z_0-9\\$\\?][A-Za-z_\\@0-9\\$\\?]*';

# variable assignment initialization
my $separator = "\t";
my $verbosity = 0;
#my $variable_fixing = 0;
#my $variable_fix_character = "_";
my $variable_to_fix_characters = "";
my $variable_fixer_characters = "";

my $register_list = " " . join(" ",@registers);

my ($infile, $outfile);
my $memory_model = "SMALL";
my $multi_index = 0;
my $comment_flag = 0;
my $inside_proc_flag = 0;
my $parsing_struc_flag = 0;
my $helped = 0;
my $use32 = 0;
my $cpu_level = 0;
my $dgroup_flag = 0;

# uninitialized general variables
my @source;
my (@parse_command, @parse_trail, @parse_lead, @parse_comment, @parse_eol);
my (@status, @trans_line);
my @multi_line;
my (@variable_names, $variable_count, @variable_type);
my (%struc_names, $current_struc_name, $struc_member_index, %struc_instance);
my $comment_delimiter;
my $proc_size;
my $start_label;
my (@macro_type_stack, %macro_passed_vars, @rep_param_stack, @macro_local_var_store);
my $last_proc_name;
my $last_label_name;
my @segment_name_stack;
my @dgroup_segments;
my (@post_regexp, @pre_regexp);

# end globals


# start the program
main();

sub main
{
	preamble();
	get_filenames();
	get_options();
	input_valid_check();
	load_source($infile, \@source);
	process_input();
	output_translation();
}

sub get_filenames
{
	foreach (@ARGV)
	{
		if (substr($_, 0, 1) ne "-")
		{
			if (!defined($infile))
			{
				$infile = $_;
			}
			elsif (!defined($outfile))
			{
				$outfile = $_;
				last;
			}
			else
			{
				last;
			}
		}
	}
}

sub get_options
{
	foreach (@ARGV)
	{
		if (substr($_, 0, 1) eq "-")
		{
			my $option = substr($_,1);
			if ($option eq "h")
			{
				# help
				help();
			}
			elsif (lc(substr($option, 0, 1)) eq 'p')
			{
#				$variable_fixing = 1;
				# fix $ start of variable with replacement
#				if (length($option) > 1 && substr($option, 1, 1) ne "\$")
#				{
#					$variable_fix_character = substr($option, 1, 1);
#				}
				if (length($option) > 1)
				{
					# must be valid fix character of $ or ?
					if (substr($option, 1, 1) eq "\$" || substr($option, 1, 1) eq "?")
					{
						$variable_to_fix_characters .= substr($option, 1, 1);
						if (length($option) > 2)
						{
							$variable_fixer_characters .= substr($option, 2, 1);
						}
						else
						{
							$variable_fixer_characters .= "_";
						}
					}
				}
				else
				{
					# default to $_
					$variable_to_fix_characters .= "\$";
					$variable_fixer_characters .= "_";
				}
			}
			elsif (lc(substr($option, 0, 1)) eq 's')
			{
				# declare separator character between replacement commands
				if (lc(substr($option, 1, 1)) eq 's')
				{
					$separator = " ";
				}
				elsif  (lc(substr($option, 1, 1)) eq 't')
				{
					$separator = "\t";
				}
			}
			elsif (lc(substr($option, 0, 1)) eq 'v')
			{
				$verbosity = 1;
			}
			elsif (lc(substr($option, 0, 1)) eq 'x')
			{
				my $pushpos = 1;
				my $pushlen = length($option) - 1;
				my $pre_flag = 0;
				if (lc(substr($option, 1, 1)) eq 'r')
				{
					$pushpos++;
					$pushlen--;
					$pre_flag = 1;
				}
				if ($option =~ m/xr?['"]/)
				{
					$pushpos++;
					$pushlen -= 2;
				}
				if ($pre_flag)
				{
					push (@pre_regexp, substr($option, $pushpos, $pushlen));
				}
				else
				{
					push (@post_regexp, substr($option, $pushpos, $pushlen));
				}
			}
			else
			{
				die "$fatal_message Bad option $_ $stopped_message";
			}
		}
	}
}

sub preamble
{
	print "\nNOMYSO, $project, Version $version.\n";
	print "Copyright $copyright_years Michael Devore.\n";
	print "NOMYSO programs released under the OSI-approved Artistic License.\n";
	print "Read LICENSE.TXT included with the NOMYSO distribution for license details.\n\n";
}

sub help
{
	print "Usage: NOMYSO [options] input_file output_file\n\n";
	print "Options:\n";
	print "  -p[char]  change symbols Prefixed with '\$' to [char], default '_'\n";
	print "  -s[s|t]   Separator for command replacements\n";
	print "             's' = space, 't' = tab, default 't'\n";
	print "  -v        Verbose feedback\n";
	print "  -x[\'\"]/pattern/replace/mod[\'\"]  post-process single lines with s// regeXp\n";
	print "  -xr[\'\"]/pattern/replace/mod[\'\"] pRe-process single lines with s// regexp\n";
	$helped = 1;
}

sub input_valid_check
{
	if (!defined($infile))
	{
		if (!$helped)
		{
			help();
		}
		$! = 255;
		die "$fatal_message No input file. $stopped_message";
	}

	if (!defined ($outfile))
	{
		$! = 255;
		die "$fatal_message No output file. $stopped_message";
	}

	open OUTFILE, ">" . $outfile or
		die "$fatal_message Could not open output file $outfile. $stopped_message";
}

sub load_source
{
	my $fhin = $_[0];
	my $source_line = $_[1];

	local *INFILE;
	open INFILE, $fhin or
		 die "$fatal_message Could not open input file $fhin. $stopped_message";

	if ($verbosity)
	{
		print "Loading source file: $fhin\n";
	}
	my $count = 0;
	while (<INFILE>)
	{
		$source_line->[$count++] = $_;
	}
}

sub process_input
{
	if ($verbosity)
	{
		print "Preprocessing source file...\n";
	}

	my $line_count = @source;
	line_breaker(\@source, $line_count,
		\@parse_command, \@parse_trail,
		\@parse_lead, \@parse_comment, \@parse_eol);

	$variable_count = 0;
	first_pass();

	# process the source lines
	$line_count = @source;	# count might have changed inside first_pass()
	for (my $index = 0; $index < $line_count; $index++)
	{
		if ($verbosity)
		{
			my $len = length($index);
			my $repeat = $len + 18;
			my $count = $index + 1;
			print "Lines processed: $count", "\x08" x $repeat;
		}
		$status[$index] = process_line($parse_command[$index], $index);
	}
}

sub line_breaker
{
	my ($source, $line_count,
		$parse_command, $parse_trail,
		$parse_lead, $parse_comment, $parse_eol) = @_;

	# read source lines and break into main parts
	for (my $index = 0; $index < $line_count; $index++)
	{
		$parse_command->[$index] = $source->[$index];
		$parse_command->[$index] =~ s {
			\s*
			(
				[^\r\n;"']*
				(?:"[^\r\n]*")?
				(?:'[^\r\n]*')?
				[^\r\n;]*
			)
			.*
			[\r\n]*
		}
		{$1}x;

		# extract trail from parse line
		$parse_trail->[$index] = $parse_command->[$index];
		$parse_trail->[$index] =~ s/.*\S(\s*)[\r\n]*/$1/;

		# remove trail from parse line
		$parse_command->[$index] =~ s/(.*\S).*[\r\n]*/$1/;

		$parse_lead->[$index] = $source->[$index];
		$parse_lead->[$index] =~ s/([\t ]*).*[\r\n]*/$1/;

		$parse_comment->[$index] = $source->[$index];
		$parse_comment->[$index] =~ s {
			[^\r\n;"']*
			(?:"[^\r\n]*")?
			(?:'[^\r\n]*')?
			[^\r\n;]*
			(;*[^\r\n]*)
			[\r\n]*
		}
		{$1}x;

		$parse_eol->[$index] = $source->[$index];
		$parse_eol->[$index] =~ s/[^\r\n]*([\r\n]*)/$1/;
	}
}

# scan source lines for all variables and start address
# auto-merge include files
sub first_pass
{
	my $line_count = @source;

	$parsing_struc_flag = 0;
	for (my $index = 0; $index < $line_count; $index++)
	{
		if ($parse_command[$index] =~ m/(?!.*('|"))\bends\b/i)
		{
			$parsing_struc_flag = 0;
			next;
		}
		elsif ($parse_command[$index] =~ m {
			(?!.*
				(
					'|"
				)
			)
			(
				(\S+)\s+
			)?
			\bstruc\b
			(
				\s+(\S+)
			)?
		}ix)
		{
			$current_struc_name = (defined($3) ? $3 : (defined($5) ? $5 : ""));
			$parsing_struc_flag = 1;
			next;
		}

		# grab variables
		if ($parse_command[$index] =~ m {
			^
			(
				$valid_symbol_pattern
			)
			\s+
			d
			(
				b|w|d
			)
			\s+
			\S+
		}iox)
		{
			$variable_type[$variable_count] = uc($2);
			# structures use fully qualified <name>.<member> names
			if ($parsing_struc_flag)
			{
				$variable_names[$variable_count++] = "$current_struc_name.$1";
			}
			else
			{
				$variable_names[$variable_count++] = $1;
			}
			next;
		}
		# handle both masm and ideal mode with leading or following label name
		elsif ($parse_command[$index] =~ m {
			^
			(
				(
					($valid_symbol_pattern)
					\s+
					label
				)
				|
				(
					label
					\s+
					($valid_symbol_pattern)
				)
			)
			\s+
			(byte|word|dword)
		}iox)
		{
			$variable_type[$variable_count] = uc(substr($6,0,1));
			my $label;
			if (defined($3))
			{
				$label = $3;
			}
			else
			{
				$label = $5;
			}
			if ($parsing_struc_flag)
			{
				$variable_names[$variable_count++] = "$current_struc_name.$label";
			}
			else
			{
				$variable_names[$variable_count++] = $label;
			}
			next;
		}
		elsif ($parse_command[$index] =~
			m/^(extrn\s+)$valid_symbol_pattern/io)
		{
			pos $parse_command[$index] = length($1);	# position past extrn command
			while ($parse_command[$index] =~
				m/($valid_symbol_pattern)(\s*:\s*([A-Za-z]+))?/go)
			{
				if (defined($3))
				{
					$variable_type[$variable_count] = uc(substr($3,0,1));
				}
				else
				{
					$variable_type[$variable_count] = 0;
				}
				if ($parsing_struc_flag)
				{
					$variable_names[$variable_count++] = "$current_struc_name.$1";
				}
				else
				{
					$variable_names[$variable_count++] = $1;
				}
			}
			next;
		}
		# check for struc instantiation variable
		elsif ($parse_command[$index] =~
			m/^($valid_symbol_pattern)\s+($valid_symbol_pattern)/o)
		{
			my $var_flag = 0;
			my $var1 = $1;
			my $var2 = $2;
			my $upper1 = uc($1);
			my $upper2 = uc($2);
			foreach my $variable (@variable_names)
			{
				if ($variable =~ m/(.*)\..*/)
				{
					if (defined($1))
					{
						my $matcher = uc($1);
						if ($matcher eq $upper1)
						{
							$variable_type[$variable_count] = 0;
							$variable_names[$variable_count++] = $var2;
							$var_flag = 1;
							last;
						}
						elsif ($matcher eq $upper2)
						{
							$variable_type[$variable_count] = 0;
							$variable_names[$variable_count++] = $var1;
							$var_flag = 1;
							last;
						}
					}
				}
			}
			if ($var_flag)
			{
				next;
			}
		}

		if ($parse_command[$index] =~
			m/^include(?:\s+)(?:\")?([^ \t\"]+)(?:\")?/i)
		{
			my $inc_file = $1;
			if ($infile =~ m{(.*(/|\\)).*})
			{
				$inc_file = $1 . $inc_file;
			}

			# read include file into local arrays
			my @inc_source;
			load_source($inc_file, \@inc_source);

			# preprocess to same status as main file
			my (@inc_command, @inc_trail, @inc_lead, @inc_comment, @inc_eol);
			my $inc_line_count = @inc_source;
			line_breaker(\@inc_source, $inc_line_count,
				\@inc_command, \@inc_trail,
				\@inc_lead, \@inc_comment, \@inc_eol);

			# splice the new include file info into existing source
			# overwrite include command with first line of include file
			splice(@source, $index, 1, @inc_source);
			splice(@parse_command, $index, 1, @inc_command);
			splice(@parse_trail, $index, 1, @inc_trail);
			splice(@parse_lead, $index, 1, @inc_lead);
			splice(@parse_comment, $index, 1, @inc_comment);
			splice(@parse_eol, $index, 1, @inc_eol);
			$line_count = @source;	# update count of lines
		}
		elsif ($parse_command[$index] =~
			m/^end\s+($valid_symbol_pattern)/io)
		{
			$start_label = $1;
		}

	}
	$parsing_struc_flag = 0;
}

sub process_line
{
	my $parse_command = $_[0];
	if (!defined($parse_command))
	{
		return UNKNOWN;
	}

	my $index = $_[1];
	my $status = 0;

	# comment checks come before any other processing
	if ($comment_flag)
	{
		if (index($parse_command, $comment_delimiter) >= 0)
		{
			$comment_flag = 0;
		}
		return COMMENTED | PROCESSED;
	}

	# catch quoted strings and don't process them
	if ($parse_command =~ m/^(?:'|")/)
	{
		return UNKNOWN;
	}

	# db strings
	my $db_string_flag = 0;
	if ($parse_command =~ m {
		(?:
			^$valid_symbol_pattern
		)?
		\s*:?\s*
		DB
		\s+
		(
			.*
			(?:'[^']*')
			|
			(?:"[^"]*")
			.*
		)
	}iox)
	{
		my $db_string = $1;

		# try to find constructs which may require processing intermixed with quoted strings
		if ($db_string =~ m {
			# catch non-matching contiguous quotes, they need further processing
			(
				('[^']*'')
				|
				("[^"]*"")
			)
			|
			(
				# find unquoted position
				(
					('[^']*')
					|
					("[^"]*")
				)*
				[^'"]*
				# unquoted dup needs further processing
				\bDUP\b
			)
		}ix)
		{
			$db_string_flag = 1;
		}
		else
		{
			# DB quoted strings don't match known constructs needing further processing
			return UNKNOWN;
		}
	}

	if ($db_string_flag)
	{
		# perform only those processes which affect DB strings
		$status = 0;
		$status |= process_doubled_quotes(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);
		$status |= command_dx(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);
		$status |= process_dup(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);
		if ($status)
		{
			# processed flag may or may not be set, since processing done, set it
			return ($status | PROCESSED);
		}
	}

	$status = 0;
	foreach my $perform (@pre_regexp)
	{
		my $regexp = "s" . $perform;
		$_ = $parse_command;
		if (eval($regexp))
		{
			$trans_line[$index] = $_;
			$status |= TRANSLATED;
		}
	}
	# keep track of last label used for local labels with no associated PROC
	track_last_label(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);

	# perform translations which end processing for that line if found
	# single line -> single or no line output
	$status |= simple_substs_end(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);
	if ($status & PROCESSED)
	{
		return $status;
	}

	# perform translations which may fall through to other translations
	# change data initializations which use DWORD and QWORD/REAL8 to DD and DQ, perform before bracket cleanup
	$status |= fix_data_inits(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);

#	# remove any commands which are superfluous before bracket cleanup (currently only xmmword)
#	$status |= remove_superfluous(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);

	# special case change qword ptr [symop] to [qword symop]
	$status |= qword_special_process(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);

	# perform bracket and plus cleanup before symbol formatting which depends on single brackets
	$status |= brackets_and_pluses(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);

	# perform symbol translation before structure data processing
	$status |= format_lines(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);

	$status |= process_struc_data(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);
	$status |= simple_substs_continue(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);
	if ($status & PROCESSED)
	{
		# unnamed proc can end processing
		return $status;
	}

	$status |= process_local_labels(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);

	# process individual opcodes that may need reformatting
	$status |= process_opcodes(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);

	$status |= process_equ_equal(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);
	$status |= process_nonreg_overrides(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);
	$status |= process_doubled_quotes(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);
	$status |= process_double_colon_label(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);
	
	# process oddball constructs of unclassified type
	$status |= process_oddball(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);

	# remove extraneous ds: overrides
	$status |= strip_extra_ds(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);

	# macro stuff comes last of single line changes, since formatting changes can affect previous processing
	#  specifically removal of the angle brackets
	$status |= process_macro_related(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);
	if ($status & PROCESSED)
	{
		# local line in macro ends processing
		return $status;
	}

	# single line -> multiple line output
	# multi-line translations come last, they also end processing
	$status |= process_dup(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);
	if ($status & PROCESSED)
	{
		return $status;
	}
	$status |= multiline_substs(($status & TRANSLATED) ? $trans_line[$index] : $parse_command, $index, $status);
	if ($status & PROCESSED)
	{
		return $status;
	}

	foreach my $perform (@post_regexp)
	{
		my $regexp = "s" . $perform;
		if ($status & TRANSLATED)
		{
			$_ = $trans_line[$index];
			if (eval($regexp))
			{
				$trans_line[$index] = $_;
			}
		}
		else
		{
			$_ = $parse_command;
			if (eval($regexp))
			{
				$parse_command = $_;
				$status |= TRANSLATED;
			}
		}
	}

	if ($status)
	{
		# processed flag may or may not be set, since processing done, set it
		return ($status | PROCESSED);
	}
	return UNKNOWN;
}

# simple substitution which end processing of line
sub simple_substs_end
{
	my $parse_command = $_[0];
	my $index = $_[1];

	my %simple_dispatch_end = qw(
		^\\.code\\b			&command_code
		^comment\s+\\S			&command_comment
		^\\.?const\\b			&command_data_const
		^(\\.|P)[1-6]86P?\\b		&command_cpu
		^\\.?data\\b			&command_data_const
		^endm\\b			&command_endm
		^[^'"]*\\bendp\\b		&command_endp
		^even\\b			&command_even
		^include\\s+\\S+		&command_include
		^\\.model\\s+\\S+		&command_model
		^[^'"]*\\bsegment\\b		&command_segment
		^assume\\b			&command_ignore
		^ideal\\b			&command_ignore
		^locals\\b			&command_ignore
		^masm\\b			&command_ignore
		^name\\b			&command_ignore
		^page\\b			&command_ignore
		^\\.seq\\b			&command_ignore
		^title\\b			&command_ignore
	);

	# dispatch to appropriate function for simple substitutions
	foreach my $pattern (keys (%simple_dispatch_end))
	{
		if ($parse_command =~ m/$pattern/i)
		{
			my $status = eval $simple_dispatch_end{$pattern};
			if (defined($status))
			{
				return $status;
			}
		}
	}

	# conditional substitutions

	if ($parse_command =~ m/\bends\b/i)
	{
		if ($parsing_struc_flag)
		{
			# ends for end of structure
			return struc_end($parse_command, $index);
		}
		else
		{
			# ends for segments are ignored and removed
			return IGNORED | REMOVED | PROCESSED;
		}
	}

	return UNKNOWN;
}

sub struc_end
{
	my $parse_command = $_[0];
	my $index = $_[1];

	my $extra = $parse_command;
	$extra =~ s/(.*)\bends\b(.*)/$1$2/i;
	$parsing_struc_flag = 0;
	$trans_line[$index] = "endstruc";
	if ($extra =~ m/\S+/)
	{
		$trans_line[$index] .=  $separator . ";" . $extra;
	}
	return TRANSLATED | PROCESSED;
}

sub track_last_label
{
	my $parse_command = $_[0];

	if ($parse_command =~ m /^($valid_symbol_no_local):/i)
	{
		$last_label_name = $1;
	}
	return;
}

# Problem: TASM local labels are scoped to procedures, whereas
#  NASM local labels are reset after each nonlocal label is encountered,
#  that is, they are scoped to the current nonlocal label.
# This means that a TASM procedure which has multiple entry points or
#  otherwise intermixes global and local labels will not work with
#  NASM local labels even if those labels are fully qualified to a
#  last known nonlocal.
# Solution is to remove local labelling, while keeping a consistent naming
#  format that doesn't remove local label information, i.e.
#  @@local: becomes @@procname_local:
# The only other reasonable solution is to track all nonlocal labels and
#  compute the appropriate fully qualified name relative to the controlling 
#  nonlocal symbol, which would lead to less consistent naming and be much
#  harder to process.
# If processing a macro, then change @@label to %%@@label
sub process_local_labels
{
	my $parse_line = $_[0];
	my $index = $_[1];
	my $status = $_[2];
	my $local_id;

	my $processing_macro = scalar(@macro_type_stack);
#	if (!$processing_macro && (!defined($last_proc_name) || !length($last_proc_name)))
#	{
#		return $status;
#	}
	if (!defined($last_proc_name) || !length($last_proc_name))
	{
		$local_id = $last_label_name;
	}
	else
	{
		$local_id = $last_proc_name;
	}

	if ($parse_line =~ m {
		^
		(?:
			@@
			(
				$valid_symbol_no_restrictions
			)
		)
		(
			\s*:
			(?:$|\s+.*)
		)
	}ox)
	{
		if ($processing_macro)
		{
			$trans_line[$index] = "%%@@" . $1 . $2;
		}
		else
		{
			$trans_line[$index] = "@@" . $local_id . "_" . $1 . $2;
		}
		$status |= TRANSLATED;
	}
	elsif ($parse_line =~ m {
		(
			.*
			[^A-Za-z0-9_\@\$\?]
		)
		@@
		(
			$valid_symbol_no_restrictions
			(?:$|[^A-Za-z0-9_\@\$\?])
			.*
		)
	}ox)
	{
		if ($processing_macro)
		{
			$trans_line[$index] = $1. "%%@@" . $2;
		}
		else
		{
			$trans_line[$index] = $1 . "@@" . $local_id . "_" . $2;
		}
		$status |= TRANSLATED;
	}

	return $status;
}

# change data initializations which use DWORD and QWORD to DD/DQ
sub fix_data_inits
{
	my $parse_line = $_[0];
	my $index = $_[1];
	my $status = $_[2];

	if ($parse_line =~ m/(\s*)($valid_symbol_pattern\s+)?(D|Q)WORD(\s+(\S+).*)/i)
	{
		if ((defined ($5)) && (uc($5) ne 'PTR'))
		{
			# convert DWORD or QWORD since not followed by PTR
			$trans_line[$index] =  (defined($1) ? $1 : "") .  (defined($2) ? $2 : "") . 'D' . uc($3) . $4;
			$status |= TRANSLATED;
		}
	}
	elsif ($parse_line =~ m/(\s*)($valid_symbol_pattern\s+)?REAL8(\s+(\S+).*)/i)
	{
		$trans_line[$index] =  (defined($1) ? $1 : "") .  (defined($2) ? $2 : "") . 'DQ' . $3;
		$status |= TRANSLATED;
	}

	return $status;
}

## remove any commands which are superfluous before bracket cleanup (currently only xmmword)
#sub remove_superfluous
#{
#	my $parse_command = $_[0];
#	my $index = $_[1];
#	my $status = $_[2];
#
#	if ($parse_command =~ m/^(.*(?:\s|,))xmmword\s(?:\s*ptr\s)?(.*)/i)
#	{
#		$trans_line[$index] = $1 . $2;
#		$status |= TRANSLATED;
#	}
#	return $status;
#}

# special qword processing, change qword ptr [symop] to [qword symop]
sub qword_special_process
{
	my $parse_command = $_[0];
	my $index = $_[1];
	my $status = $_[2];

	if ($parse_command =~ m/^(.*(?:\s|,))(qword\s+)ptr\s+(\[)(.*)/i)
	{
		$trans_line[$index] = $1 . $3 . $2 . $4;
		$status |= TRANSLATED;
	}
	return $status;
}


# process multiple brackets and extra '+' signs into single bracket
# also place indexed references, e.g. moo[edi] or moo[56], into single bracket
sub brackets_and_pluses
{
	my $parse_line = $_[0];
	my $index = $_[1];
	my $status = $_[2];

	my $test_line = $parse_line;
	my $lead = "";
	if ($test_line =~ s/((?:^\S*\s*:)?(?:^|\s*)(\S+)\s+)(.*)/$3/ == 0)
	{
		return $status;
	}
	if (defined ($1))
	{
		$lead = $1;
	}

	if ($test_line =~ m/^(EQU\s+|=\s*)(.*)/i)
	{
		$lead .= $1;
		$test_line = $2;
	}

	# filter out lines which normally allow multiple bracketing or pluses
	my $first_op = $2;
	if ($first_op =~ m/^(rep(nz|ne|e|z)?|movs|stos|lods|scas|REPT)$/i)
	{
		return $status;
	}

	# filter out lines which normally allow multiple bracketing or pluses
#	if ($test_line =~ m/^(movs|stos|lods|scas|DB|DW|DD|EQU|=)\s+/i)
	if ($test_line =~ m/^(movs|stos|lods|scas|DB|DW|DD)\s+/i)
	{
		return $status;
	}
	if ($test_line =~ m/<.*>/)
	{
		return $status;
	}

	$test_line =~ s {
		(
			(?:
				$valid_symbol_pattern
				\s*:\s*
			)?
			(?:
				(?:
					.*,
				)?
				\s*
				\b(?:
#					PWORD|FWORD|DWORD|WORD|TBYTE|TWORD|BYTE|SHORT
					XMMWORD|QWORD|PWORD|FWORD|DWORD|WORD|TBYTE|TWORD|BYTE|SHORT
				)\b
				(?:
					\s*
					\bPTR\b
				)?
			)?
			\s*
		)?
		(.*)
	}{$2}iox;
	if (defined($1))
	{
		$lead .= $1;
	}

	my $change_needed = $test_line =~ s {
		(.*,\s*)?
		(
			.*
			(?:
				# +\[
				(?:\+[^,]*\[)
				|
				# ]+
				(?:\][^,]*\+)
				|
				# ]\[
				(?:\][^,]*\[)
				|
#				# + symbol
#				(?:\+\s*$valid_symbol_pattern)
#				|
#				# symbol +
#				(?:\b$valid_symbol_pattern\s*\+)
#				|
				# symbol [
				(?:\b$valid_symbol_pattern\s*\[)
			)
			.*?
		)
		(\s*(,|$).*)
	}{$2}ox;
	if (!$change_needed)
	{
		return $status;
	}

	my $trail = "";
	if (defined($1))
	{
		$lead .= $1;
	}
	if (defined($3))
	{
		$trail = $3;
	}

	# check if $test_line is $- or $+ and reject formatting if so
	if ($test_line =~ m/^\$(-|\+)/)
	{
		return $status;
	}
	
	# put left bracket at front, changing subsequent brackets to plus signs,
	#	removing contiguous plus signs, and right bracket at end or right before a comma
	my $mod_line = "[";
	my $pos = 0;
	if (substr($test_line, 0, 1) eq "[")
	{
		$pos++;
	}
	my $plus_flag = 1;	# initial state has bracket (equivalent to plus)
	my $char;
	my $nouse;
	my $len = length ($test_line);
	while ($len--)
	{
		$char = substr($test_line, $pos, 1);
		$nouse = 0;
		if ($char eq "+")
		{
			if ($plus_flag)
			{
				$nouse = 1;
			}
			else
			{
				$plus_flag = 1;
			}
		}
		elsif ($char eq "[")
		{
			if (!$plus_flag)
			{
				$char = "+";
				$plus_flag = 1;
			}
			else
			{
				$nouse = 1;
			}
		
		}
		elsif ($char eq "]")
		{
			if (!$plus_flag)
			{
				$char = "+";
				$plus_flag = 1;
			}
			else
			{
				$nouse = 1;
			}
		}
		elsif ($char eq ",")
		{
			$mod_line .= "]" . substr($test_line, $pos);
			last;
		}
		elsif ($char eq ":")
		{
			# don't convert :[ to :+
			$plus_flag = 1;
		}
		elsif ($char gt " ")
		{
			$plus_flag = 0;
		}
		if (!$nouse)
		{
			$mod_line .= $char;
		}

		$pos++;
	}
	$char = substr($mod_line, -1, 1);
	if ($char eq "+")
	{
		substr($mod_line, -1, 1) = "]";
	}
	elsif (substr($mod_line, -1, 1) ne "]")
	{
		$mod_line .= "]";
	}

	$trans_line[$index] = $lead . $mod_line . $trail;
	$status |= TRANSLATED;

	return $status;
}

# bracket symbols and add size, as necessary
# move any segment overrides inside brackets
# change '$' prefix on symbol if flagged
sub format_lines
{
	my $parse_line = $_[0];
	my $index = $_[1];
	my $status = $_[2];

	my (@components, @comp_pos);
	my $counter = 0;
	while ($parse_line =~ m {
		(
			# comma or
			,
			|
			# single quoted string or
			'[^']*'
			|
			# double quoted string or
			"[^"]*"
			|
			(?:
				# optional segment override
				(?:$valid_symbol_pattern\s*:\s*)?
				(
			# left bracket or
					(?:
						\[
					)
					|
			# symbol/constant, including leading period
					(?:
						[A-Za-z_0-9\@\$\?]+
						#  plus optional structure . symbol
						(?:
							\s*\.\s*$valid_symbol_pattern
						)?
					)
					|
			# leading period structure symbol
					(?:
						\s*\.\s*$valid_symbol_pattern
					)
				)
			)
		)
	}gox)
	{
		my $uc1 = uc($1);
		if (($counter < 2) && ($uc1 eq "PROC" or $uc1 eq "LABEL"))
		{
			# don't process PROC/label lines
			return $status;
		}
		if (!$counter && ($uc1 eq "REPT"))
		{
			# don't process REPT lines
			return $status;
		}
		push (@components, $1);
		$comp_pos[$counter++] = pos($parse_line);
	}
	my $comp_len = @components;

	my $add_bracket_flag = 0;
	my $bracket_exists_flag = 0;
	my $calljmp_flag = 0;
	my $closed_bracket_flag = 0;
	my $comma_flag = 0;
	my $no_bracket_flag = 0;
	my $offset_flag = 0;
	my $register_flag = 0;
	my $size_override_register_flag = 0;	# movzx movsx
	my $size_flag = 0;
	my $existing_bracket_pos = 0;
	my $type = 0;
	my $adjust = 0;
	my $changed = 0;
	my $override = "";
	my ($upper, $len);
	$counter = 0;

	foreach my $component (@components)
	{

		if ($component eq "\$")
		{
			# don't process solitary '$'
			$no_bracket_flag = 1;	# assume $ manipulations won't involve brackets (using direct values)
			next;
		}

		$upper = uc($component);
		if ($upper eq "CALL" or $upper eq "JMP")
		{
			# track for CALL FAR [cs:var] hack
			$calljmp_flag = 1;
			next;
		}

		if ((index($register_list, " " . $upper) >= 0))
		{
			if (!$bracket_exists_flag && !$size_override_register_flag)
			{
				# set register flag only if not used in a bracketed context
				$register_flag = 1;
			}
			next;
		}
	
		if (!$counter && (($upper eq "PUBLIC") || ($upper eq "EXTRN")))
		{
			$no_bracket_flag = 1;
			next;
		}
		if (($counter == 1) && $upper eq "EQU")
		{
#			$no_bracket_flag = 1;
			next;
		}
		if (!$counter && ($upper eq "OUT"))
		{
			# don't bracket out instructions
			$no_bracket_flag = 1;
			next;
		}
		if (!$counter && !scalar(@macro_type_stack) && !scalar(@rep_param_stack))
		{
			# check for macros, which would inhibit a bracketing
			my $match = 0;
			my $counter = 0;
			foreach my $macro (keys (%macro_passed_vars))
			{
				if ($upper eq uc($macro))
				{
					$match = 1;
					last;
				}
			}
			if ($match)
			{
				$no_bracket_flag = 1;
				next;		
			}
		}
		if ($upper eq "SEG")
		{
			$no_bracket_flag = 1;
			next;
		}
		if ($upper =~ m/\bD[BWD]\b/)
		{
			$no_bracket_flag = 1;
			next;
		}
 
		if (($upper eq "MOVZX") || ($upper eq "MOVSX") ||
			($upper eq "SHR") || ($upper eq "SHL"))
		{
			# need a size specification with these instructions even if a register is listed
			$size_override_register_flag = 1;
		}

		# modify lines based on component content and flag status
		$len = length($component);

		my $stripped = $component;
		my $override_flag = 0;
		# colon can't be first part of line, to rule out labels as segment overrides
		if ($counter && index($component, ":") >= 0)
		{
			# process segment override prefix
			$stripped =~ s/(.+:)\s*(.+)/$2/;
			$override = $1;
			$override =~ tr/ //;
			$override_flag = 1;
		}
		$stripped = uc($stripped);

		if ($stripped eq "BYTE" or $stripped eq "WORD" or $stripped eq "DWORD" or
			$stripped eq "PWORD" or $stripped eq "FWORD" or
			$stripped eq "QWORD" or
			$stripped eq "TWORD" or $stripped eq "TBYTE" or
			$stripped eq "NEAR" or $stripped eq "FAR")
		{
			# if this directly followed an override, remove the override if not in brackets
			if ($override_flag)
			{
				if (!$add_bracket_flag && !$bracket_exists_flag && !$no_bracket_flag)
				{
					my $remove_len = length($component) - length($stripped);
					substr($parse_line, $comp_pos[$counter] - $len + $adjust, $remove_len) = "";
					$adjust -= $remove_len;
				}
				$changed = 1;
			}

			$size_flag = 1;
			next;
		}

		if ($component eq ",")
		{
			$comma_flag = 1;
			if ($add_bracket_flag || $bracket_exists_flag)
			{
				# reset register flag only if used in a bracketed context
				$register_flag = 0;
			}
			# if bracketing variable, close the bracket, reset formatting flags
			if ($add_bracket_flag)
			{
				substr($parse_line, $comp_pos[$counter] - 1 + $adjust, 0) = "]";
				$closed_bracket_flag = 1;
				$adjust++;
				$changed = 1;
			}
			# reset various flags associated with one side of a comma
			$offset_flag = 0;
			$size_flag = 0;
			$type = 0;
			$override = "";
			next;
		}

		# remove PTR even when associated with NEAR and FAR labels
		if ($counter && ($upper eq "PTR"))
#		if ($counter && $size_flag && ($upper eq "PTR"))
		{
			# remove PTR and one leading whitespace
			substr($parse_line, $comp_pos[$counter] - $len - 1 + $adjust, $len + 1) = "";
			$adjust -= $len + 1;
			$changed = 1;
			next;
		}

		if ($counter && ($upper =~ m/(?:[cdefgs]s:)?\b(OFFSET)/i))
		{
			# remove OFFSET and one trailing whitespace
			my $remove = length($1);	# keep segment override if present
			$offset_flag = 1;
			substr($parse_line, $comp_pos[$counter] - $remove + $adjust, $remove + 1) = "";
			$adjust -= $remove + 1;
			$changed = 1;
			next;
		}

		# treat LOWWORD as OFFSET removal
		#  (NASM has no reasonable alternative to LOWWORD)
		if ($counter && ($upper =~ m/(?:[cdefgs]s:)?\b(LOWWORD)/i))
		{
			# remove LOWWORD
			my $remove = length($1);	# keep segment override if present
			$offset_flag = 1;
			substr($parse_line, $comp_pos[$counter] - $remove + $adjust, $remove + 1) = "";
			$adjust -= $remove + 1;
			$changed = 1;
			next;
		}

		if (substr($component, $len - 1, 1) eq "[")
		{
			$existing_bracket_pos = $comp_pos[$counter] + $adjust - 1;
			# check for segment override which must go inside bracket
			my $add_len = length($override);
			if ($add_len)
			{
				my $remove_len = length($component) - length($stripped);
				if ($remove_len)
				{
					# remove override from old position
					substr($parse_line, $comp_pos[$counter] - $len + $adjust, $remove_len) = "";
					$adjust -= $remove_len;
					$existing_bracket_pos -= $add_len;	# moved the bracket position, adjust
				}

				# add override to new position
				substr($parse_line, $comp_pos[$counter] + $adjust, 0) = $override;
				$adjust += $add_len;
				$changed = 1;
			}
			$bracket_exists_flag = 1;
			# don't skip to next because it may need a type specifier
		}

		my $found_var = 0;

		# no format modifications to symbol which starts line, or
		#  is currently being bracketed, or
		#  comes after PUBLIC/EXTRN/DB/DW/DD, or
		#  or same side as OFFSET of comma
		if ($counter &&
			!$add_bracket_flag && !$no_bracket_flag &&
			!$offset_flag)
		{
			my ($struc_name, $struc_member);
			my $valid_name = 0;
			my $dotted = 0;
			$struc_member = $struc_name = $upper;

			if (index($component, ".") >= 0)
			{
				$dotted = 1;
				$struc_member = $struc_name = $stripped;

				# break structure into name and member
				$valid_name = $struc_name =~ s/.*?($valid_symbol_pattern)\s*\..*/$1/o;
				$struc_member =~ s/.*\.\s*($valid_symbol_pattern).*/$1/o;
				# there may be no structure name if bare dotted member
				# so account for that when attempting to get $type via $valid_name flag
			}
			if (!$valid_name && $dotted)
			{
				foreach my $name (keys %struc_names)
				{
					my $full_name = uc($name) . "." . $struc_member;
					my $var_index = 0;
					foreach my $variable (@variable_names)
					{
						if (uc($variable) eq $full_name)
						{
							$type = $variable_type[$var_index];
							$found_var = 1;
							last;
						}
						$var_index++;
					}
				}
			}
			elsif ($dotted)
			{

				foreach my $key (keys %struc_instance)
				{
					if (uc($key) eq $struc_name)
					{
						# fully qualify $struc_member name
						my $full_name = uc($struc_instance{$key}) . "." . uc($struc_member);
						my $var_index = 0;
						foreach my $variable (@variable_names)
						{
							if (uc($variable) eq $full_name)
							{
								$type = $variable_type[$var_index];
								$found_var = 1;
								last;
							}
							$var_index++;
						}
	
						# don't keep looping if we know the structure name associated with variable
						if ($found_var)
						{
							last;
						}
					}
				}
			}

			my $var_index = 0;
			foreach my $variable (@variable_names)
			{
				if (uc($variable) eq $stripped)
				{
					$type = $variable_type[$var_index];
					$found_var = 1;
					last;
				}
				$var_index++;
			}
		}

		if ($found_var)
		{
			my $insertion = "";
			my $position;
			my $distance_flag = 0;
			if ($type eq "N" or $type eq "F")
			{
				# assume that a near or far type variable needs no bracketing
				#  but it does need a NEAR or FAR
				if ($type eq "N")
				{
					$insertion = "NEAR ";
				}
				else
				{
					$insertion = "FAR ";
				}
				$distance_flag = 1;	# flag distance-based variable
				$position = $comp_pos[$counter] - $len + $adjust;
			}
			elsif ($type && !$size_flag && !$register_flag)
			{
				# need type override only if one not already specified and
				#  a register wasn't previously specified outside of a bracket
				# bare block to allow 'last'ing an if compare
				{
					# if comma follows with a bare register, no type override
					# size override negates this test
					if (!$comma_flag && !$size_override_register_flag &&
						($counter + 2 < $comp_len))
					{
						if (($components[$counter+1] eq ",") &&
							(index($register_list, " " . uc($components[$counter+2])) >= 0))
						{
							# bypass modifications based on $type
							last;
						}
					}
	
					if ($type eq "B")
					{
						$insertion = "BYTE";
					}
					elsif ($type eq "W")
					{
						$insertion = "WORD";
					}
					elsif ($type eq "D")
					{
						# HACK Exception:
						# check for CALL/JMP DWORD [cs:var) and change
						#  DWORD to FAR

						if ($calljmp_flag && (uc($override) eq "CS:"))
						{
							$insertion = "FAR";
						}
						else
						{
							$insertion = "DWORD";
						}
					}
					$insertion .= " ";
				}
			}

			# only mess with brackets if not a distance-based variable
			if (!$distance_flag)
			{
				if (!$bracket_exists_flag)
				{
					# flag we are adding bracket
					$add_bracket_flag = 1;
					$insertion .= "[";
					$position = $comp_pos[$counter] - $len + $adjust;
				}
				else
				{
					# pre-existing bracket, position previously saved
					$position = $existing_bracket_pos;
				}
			}

			# insert proper type and/or left bracket
			if (length($insertion))
			{
				substr($parse_line, $position, 0) = $insertion;
				$adjust += length($insertion);
				$changed = 1;
			}
		}
	}
	continue
	{
		$counter++;
	}

	# if added a bracket, close the bracket
	if ($add_bracket_flag && !$closed_bracket_flag)
	{
		substr($parse_line, length($parse_line), 0) = "]";
		$changed = 1;
	}

#	if ($variable_fixing)
	if (length($variable_to_fix_characters))
	{
		if (length($variable_to_fix_characters))
		{
			my ($fixme,$withwhat);
			for (my $i = 0; $i < length($variable_to_fix_characters); $i++)
			{
				$fixme = "\\" . substr($variable_to_fix_characters, $i, 1);
				$withwhat = substr($variable_fixer_characters, $i, 1);
				$changed |=
					($parse_line =~ s/(?<=[^A-Za-z0-9\@_\$\?])($fixme)(?=[A-Za-z0-9\@_\$\?])/$withwhat/g) ? 1 : 0;
				# modify symbols at beginning of line
				$changed |= $parse_line =~ s/^($fixme)(?=[A-Za-z0-9\@_\$\?])/$withwhat/;
			}
		}
#		$changed |=
#			($parse_line =~ s/(?<=[^A-Za-z0-9\@_\$\?])\$(?=[A-Za-z0-9\@_\$\?])/$variable_fix_character/g) ? 1 : 0;
#		# modify symbols at beginning of line
#		$changed |= $parse_line =~ s/^\$(?=[A-Za-z0-9\@_\$\?])/$variable_fix_character/;
	}

	if ($changed)
	{
		$trans_line[$index] = $parse_line;
		$status |= TRANSLATED;
	}

	return $status;
}


# look for structure data of form instance.field,
#  translate to instance+field
# also look for something].field and translate to something+struct.field]
# +.field => +struct.field
sub process_struc_data
{
	my $parse_line = $_[0];
	my $index = $_[1];

	foreach my $key (keys %struc_instance)
	{
		# don't keep any leading space before field after . since NASM does not like it
		if ($parse_line =~ m/(.*\S*[^A-Za-z0-9\@_\$\?]+$key\s*)\.\s*(\S+.*)/i)
		{
			my $base_struc = $struc_instance{$key};
			$trans_line[$index] = "$1+$base_struc.$2";
			return TRANSLATED;
		}
	}

	if ($parse_line =~ m/(.*)(\]|\+)\s*\.\s*($valid_symbol_pattern)(.*)/o)
	{
		my $lead = $1;
		my $struc_member = $3;
		my $bracket = "";
		if ($2 eq ']')
		{
			$bracket = ']';
		}
		my $trail = $4;
		foreach my $name (keys %struc_names)
		{
			# fully qualify $struc_member name
			my $full_name = uc($name) . "." . uc($struc_member);
			foreach my $variable (@variable_names)
			{
				if (uc($variable) eq $full_name)
				{
					# found the base structure which contains the dotted member
					$trans_line[$index] =
						$lead .  "+" . $name . "." . $struc_member . $bracket . $trail;
					return TRANSLATED;

				}
			}
		}
	}

	return 0;
}

# simple substitution which may continue line processing
sub simple_substs_continue
{
	my $parse_command = $_[0];
	my $index = $_[1];
	my $status = $_[2];

	my %simple_dispatch_continue = qw(
		\\bd[wbd]\s+\\S+		&command_dx
		^extrn\\b			&command_extrn
		^[^'"]*\\blabel\\s+		&command_label
		^[^'"]*\\bproc\\b		&command_proc
		^public\\s+\\S+			&command_public
		^[^'"]*\\bseg\\b		&command_seg
		^\\S+\\s+struc\\b		&command_struc_trailing
		^struc\\s+\\S+			&command_struc_leading
		^if\\s+\\S+			&command_perc_prefix
		^ife\\s+\\S+			&command_ife
		^ifn?b\\s+\\S+			&command_ifb
		^ifn?def\\s+\\S+		&command_perc_prefix
		^ifidni?\\s+\\S+		&command_perc_prefix
		^elseif				&command_elseif
		^else\\b			&command_perc_prefix
		^elif\\s+\\S+			&command_perc_prefix
		^endif\\b			&command_perc_prefix
		\\btbyte\\b			&subst_tbyte
		\\bxmmword\\b			&subst_xmmword
	);

	# dispatch to appropriate function
	foreach my $pattern (keys (%simple_dispatch_continue))
	{
		if ($parse_command =~ m/$pattern/i)
		{
			$status |= eval $simple_dispatch_continue{$pattern};
		}
	}
	return $status;
}

sub process_opcodes
{
	my $parse_line = $_[0];
	my $index = $_[1];

	# ret to retn or retf as indicated by proc or model
	if ($parse_line =~ m/(^|.*\s+)ret($|\s+.*)/i)
	{
		my $which_ret;
		if ($inside_proc_flag && $proc_size)
		{
			if ($proc_size eq "FAR")
			{
				$which_ret = "retf";
			}
			else
			{
				$which_ret = "retn";
			}
		}
		elsif (($memory_model eq "MEDIUM") ||
			($memory_model eq "LARGE") ||
			($memory_model eq "HUGE"))
		{
			$which_ret = "retf";
		}
		else
		{
			$which_ret = "retn";
		}

		$trans_line[$index] = $1 . $which_ret . $2;
		return TRANSLATED;
	}

	# lods to lods[bwd]
	if ($parse_line =~ m {
		(.*)			#1
		\b
		lods
		\b
		.*
		\b
		(BYTE|WORD|DWORD)	#2
		\b
		(				#3
			.*
			\[?
			(.s)		#4
			\s*:
		)?
		(?:
			.*
			\b
			(si|esi)	#5
			\b
		)?
	}ix)
	{
		$trans_line[$index] = "";
		if (defined($5))
		{
			if ((uc($5) eq "SI") && $use32)
			{
				$trans_line[$index] = "a16" . $separator;
			}
			elsif ((uc($5) eq "ESI") && !$use32)
			{
				$trans_line[$index] = "a32" . $separator;
			}
		}

#		$trans_line[$index] .= $1;
		$trans_line[$index] = $1 . $trans_line[$index];

		if (defined($3))
		{
			$trans_line[$index] .= $4 . $separator;
		}
		if (uc($2) eq "WORD")
		{
			$trans_line[$index] .= "lodsw";
		}
		elsif (uc($2) eq "DWORD")
		{
			$trans_line[$index] .= "lodsd";
		}
		else
		{
			$trans_line[$index] .= "lodsb";
		}
		return TRANSLATED;
	}

	# stos/scas to stos/scas[bwd]
	if ($parse_line =~ m {
		(.*)		#1
		\b
		(stos|scas)	#2
		\b
		.*
		\b
		(BYTE|WORD|DWORD)	#3
		\b
		(			#4
			.*
			\[?
			(.s)	#5
			\s*:
		)?
		(?:
			.*
			\b
			(di|edi)	#6
			\b
		)?
	}ix)
	{
		$trans_line[$index] = "";
		if (defined($6))
		{
			if ((uc($6) eq "DI") && $use32)
			{
				$trans_line[$index] = "a16" . $separator;
			}
			elsif ((uc($6) eq "EDI") && !$use32)
			{
				$trans_line[$index] = "a32" . $separator;
			}
		}

		$trans_line[$index] = $1 . $trans_line[$index];

		if (defined($4))
		{
			$trans_line[$index] .= $5 . $separator;
		}
		if (uc($2) eq "STOS")
		{
			$trans_line[$index] .= "stos";
		}
		else
		{
			$trans_line[$index] .= "scas";
		}
		if (uc($3) eq "WORD")
		{
			$trans_line[$index] .= "w";
		}
		elsif (uc($3) eq "DWORD")
		{
			$trans_line[$index] .= "d";
		}
		else
		{
			$trans_line[$index] .= "b";
		}
		return TRANSLATED;
	}

	# movs to movs[bwd]
	if ($parse_line =~ m {
		(.*)		#1
		\b
		movs
		\b
		.*
		\b
		(BYTE|WORD|DWORD)	#2
		\b
		(?:
			.*
			\b
			(si|esi|di|edi)	#3
			\b
		)?
	}ix)
	{

		$trans_line[$index] = "";
		if (defined($3))
		{
			if (((uc($3) eq "SI") || (uc($3) eq "DI")) && $use32)
			{
				$trans_line[$index] = "a16" . $separator;
			}
			elsif (((uc($3) eq "ESI") || (uc($3) eq "EDI")) && !$use32)
			{
				$trans_line[$index] = "a32" . $separator;
			}
		}

		$trans_line[$index] = $1 . $trans_line[$index];

		if (uc($2) eq "WORD")
		{
			$trans_line[$index] .= "movsw";
		}
		elsif (uc($2) eq "DWORD")
		{
			$trans_line[$index] .= "movsd";
		}
		else
		{
			$trans_line[$index] .= "movsb";
		}
		return TRANSLATED;
	}

	# see if need to put a size qualifier for push/pop
	my $which_size = ($memory_model eq "FLAT") ? "DWORD" : "WORD";
	if ($parse_line =~ s {
		(
			.*(push|pop)\s+
		)
		(
			# scan past starting bracket, if any
			(\[\s*)?
#			# scan past segment override, if any
#			([A-Za-z_\$][A-Za-z_0-9_\$]*\s*:\s*)?
			\d
		)
	}
	{$1$which_size $3}ix)
	{
		$trans_line[$index] = $parse_line;
		return TRANSLATED;
	}

	# call's and jmp's may have an explicit DWORD PTR specifier with a cs:override, e.g.
	#  call DWORD PTR cs:[goof]
	#  which should be interpreted as call far cs:[value]
	#  so directly convert a call/jmp DWORD [cs:whatever] to call/jmp FAR [cs:whatever]
	#  note that this point, the cs: override is always inside brackets and PTR removed
	if ($parse_line =~ s/(.*\b(call|jmp)\s+)(DWORD)(\s*\[cs:.*)/$1FAR$4/i)
	{
		$trans_line[$index] = $parse_line;
		return TRANSLATED;
	}
	
	return 0;
}

sub process_equ_equal
{
	my $parse_line = $_[0];
	my $index = $_[1];

	# convert "symbol =/EQU non-numeric_value" to
	#  "%define symbol non-numeric_value"
	# Exception: value is considered numeric if it starts with an '$'
	#  because NASM preprocessor cannot handle %define of symbol after
	#  the symbol is used so, for example:
	#    DW moo
	#    cow DW 0
	#    %define moo $-cow
	#  doesn't work, but
	#    moo EQU $-cow 
	#  does work
	if ($parse_line =~ m {
		^
		(
			(
				$valid_symbol_pattern
			)
			\s*
			(?:
				=
				|
				\s+EQU\s+
			)
			\s*
		)
		(
			(?:
				[^0-9\$\s]
				|
				[0-9]\s*,
			)
			.*
		)
	}iox)
	{
		my $symbol = $2;
		my $value = $3;

		# change <args> to (args)
		$value =~ s/^<(.*)>/($1)/;	

		my $op = "%DEFINE";
		# if processing a macro, use %assign if looks like a numeric because of 
		#  use of +-*/ digits
		# Future change: use %ifnum construct for more intelligent processing?
		if (scalar(@macro_type_stack))
		{
			if ($value =~ m/(\+|-|\*|\/)\s*\d/)
			{
				$op = "%ASSIGN";
			}
		}

		$trans_line[$index] = $op . $separator . $symbol . $separator . $value;
		return TRANSLATED;
	}

	# convert "symbol = numeric_value" to "symbol equ numeric_value"
	# values starting with '$' are considered numeric
	if ($parse_line =~ m {
		(
			^
			$valid_symbol_pattern
		)
		\s*
		=
		\s*
		(
			(\$|\d)
			.*
		)
	}iox)
	{
		$trans_line[$index] = $1 . $separator . "EQU" . $separator . $2;
		return TRANSLATED;
	}
}

sub process_nonreg_overrides
{
	my $parse_line = $_[0];
	my $index = $_[1];

	# convert nonsegment register segment overrides from
	#  <seg>:<var> to <var>:wrt <seg>
	if ($parse_line =~ m {
		(
			# can't be first non-whitespace parameter
			# so that label:'s aren't picked up
			.*
			\S+
			\s+
			.*
		)
		# beginning of seg name
		(?<=[^.A-Za-z_0-9\@\$\?])
		# filter out segment register overrides
		(?!
			(
				cs|ds|es|fs|gs|ss
			)
			[^A-Za-z_0-9\@\$\?]
		)
		(
			$valid_symbol_pattern
		)
		\s*:\s*
		(
			$valid_symbol_pattern
			[^.\]]*
		)
		(.*)
	}iox)
	{
		my $check_line = $1 . $4 . " wrt " . $3 . $5;
		my $lead = $1;
		if (($lead =~ m/(^|\s+)db.*"\s*$/i) == 0)
		{
			$trans_line[$index] = $check_line;
			return TRANSLATED;
		}
	}
}

sub process_macro_related
{
	my $parse_line = $_[0];
	my $index = $_[1];
	my $status = $_[2];

	my $changed = 0;

	# REPT/REPEAT
	if ($parse_line =~ m/^(?:REPT|REPEAT)(\s+.*)/i)
	{
		$parse_line = "%REP" . $1;
		$changed = 1;
		push (@macro_type_stack, "%REP");	# track for ENDM processing
	}

	# changes within macro definition
	if (scalar(@macro_type_stack))
	{

		# process variables passed to macro
		foreach my $macro (keys (%macro_passed_vars))
		{
			my $counter = 1;
			foreach my $arg (@{ $macro_passed_vars{$macro} })
			{
				# don't change a FOR/IRP
				if (($parse_line =~ m/^(FOR|IRP)\s+/i) == 0)
				{
					# change variable reference to %%ref
					$changed |= $parse_line =~ s/\b($arg)\b/%%$1/gi;

					# check for label which embeds a variable
					# if found, revert variable to %1->%n equivalent as %{n}, so that NASM
					# will leave the label internally unmodified, rather than localizing it
					# remember that variable has %% prepended from previous s//
					if ($parse_line =~ m {
						^
						(?:
							(?:
								(
									$valid_symbol_pattern
								)
								&%%
								$arg
							)
							|
							(?:
								&%%
								$arg
								(
									$valid_symbol_pattern
								)
							)
						)
						\s*
						(:.*)
					}x)	# no /o modifier, as $arg varies
					{
						my $label;
						if (defined($1))
						{
							$label = $1 . "%{" . $counter . "}";
						}
						else
						{
							$label = "%{" . $counter . "}" . $2;
						}
						$parse_line = $label . $3;
						$changed = 1;
					}

					# check for DB/DW/DD which uses an embedded variable, as above
					if ($parse_line =~ m {
						^
						(
							D
							(?:B|W|D)
							\s+
						)
						(?:
							(?:
								(
									$valid_symbol_pattern
								)
								&%%
								$arg
							)
							|
							(?:
								&%%
								$arg
								(
									$valid_symbol_pattern
								)
							)
						)
						(.*)
					}ix)	# no /o modifier
					{
						my $value;
						if (defined($2))
						{
							$value = $2 . "%{" . $counter . "}";
						}
						else
						{
							$value = "%{" . $counter . "}" . $3;
						}
						$parse_line = $1 . $value . $4;
						$changed = 1;
					}
	
				}
				$counter++;
			}
		}

		# REPT/REPEAT
#		if ($parse_line =~ m/^(?:REPT|REPEAT)(\s+.*)/i)
#		{
#			$parse_line = "%REP" . $1;
#			$changed = 1;
#			push (@macro_type_stack, "%REP");	# track for ENDM processing
#		}

		# declare LOCAL variables
		if ($parse_line =~ m/^LOCAL\s+(.*)/i)
		{
			my $locals = $1;
			while ($locals =~ m/($valid_symbol_pattern)/og)
			{
				push (@ { $macro_local_var_store[$#macro_type_stack] }, $1);
			}
			# done processing this line
			return REMOVED | PROCESSED;
		}

		# change local variables to %%variable
		for (my $i = 0; $i <= $#macro_local_var_store; $i++)
		{
			if (defined($macro_local_var_store[$i]))
			{
				for (my $j = 0; $j < scalar(@{$macro_local_var_store[$i]}); $j++)
				{
					my $local_var = $macro_local_var_store[$i][$j];
					my $mod_line = $parse_line;
					$changed |= $parse_line =~ s/(.*?)\b$local_var\b/$1%%$local_var/g;
				}
			}
		}

		# change %(var) to var
		$changed |= $parse_line =~ s/(.*)%\((.*)\)/$1$2/;

	}

	if (scalar(@rep_param_stack))
	{
		foreach my $param (@rep_param_stack)
		{
			$changed |= $parse_line =~ s/\b($param)\b/%1/gi;
		}
	}

	# look for macro in regular lines, remove '<>'s concatenating arguments
	if (!scalar(@macro_type_stack) && !scalar(@rep_param_stack) &&
		$parse_line =~ m/.*<.*>/)
	{
		foreach my $macro (keys (%macro_passed_vars))
		{
			if ($parse_line =~ m/(.*\b$macro\b.*?)<(.*)>(.*)/)
			{
				$parse_line = $1 . $2 . $3;
				$changed = 1;
				last;
			}
		}
	}
	
	if ($changed)
	{
		$trans_line[$index] = $parse_line;
		return TRANSLATED;
	}

	return $status;
}

sub process_doubled_quotes
{
	my $parse_line = $_[0];
	my $index = $_[1];
	my $status = $_[2];

	# change double single-quotes and double double-quotes to comma-breaked quotes
	#  for a DB only
	my $changed = 0;
	while ($parse_line =~ m/(.*?DB.*?("|').*?(?<!\\))(\2)(\2)(.*)/i)
	{
		my $insertion = $3;
		# put hex code for quote in comma-broken string
		if ($insertion eq '"')
		{
			$insertion = "22h";
		}
		else
		{
			$insertion = "27h";
		}
		$parse_line = $1 . $3 . "," . $insertion . "," . $3 . $5;
		$changed = 1;
	}
	if ($changed)
	{
		$trans_line[$index] = $parse_line;
		return TRANSLATED;
	}

	return $status;
}

# change double colon label to single colon
sub process_double_colon_label
{
	my $parse_command = $_[0];
	my $index = $_[1];
	my $status = $_[2];

	if ($parse_command =~ m /^($valid_symbol_pattern)::(.*)/i)
	{
		$trans_line[$index] = "$1:" . (defined($2) ? $2 : "");
		$status |= TRANSLATED;
	}
	return $status;
}

sub process_oddball
{
	my $parse_line = $_[0];
	my $index = $_[1];
	my $status = $_[2];

	# convert les <reg>,DWORD to les <reg>,
	if ($parse_line =~ s/((^|\s+)les\s+\S+,)\s*DWORD\s+/$1/i)
	{
		$trans_line[$index] = $parse_line;
		$status |= TRANSLATED;	# allow cascade
	}

	# convert ,<[ NOT/AND/OR/SHR/SHL/XOR <value> to ,<[ ~/&/|/>>/<< /^ <value>
	# support same conversion if IF/%IF/%DEFINE conditional
	#  support DB/DW/DD
	my $changed = 0;
	while ($parse_line =~ m {
		(
			.*(?:
				[,<\[]
				|
				(?:^%?IF|DEFINE)
				|
				(?:D[BWD])
			).*?
		)
		\b(NOT|AND|OR|SHR|SHL|XOR)
		(
			\s+\S+.*
		)
	}ix)	# no 'g' modifier, restart from beginning
	{
		$changed = 1;
		my $new_op;
		my $old_op = uc($2);
		if ($old_op eq "AND")
		{
			$new_op = "&";
		}
		elsif ($old_op eq "OR")
		{
			$new_op = "|";
		}
		elsif ($old_op eq "SHR")
		{
			$new_op = ">>";
		}
		elsif ($old_op eq "SHL")
		{
			$new_op = "<<";
		}
		elsif ($old_op eq "XOR")
		{
			$new_op = "^";
		}
		else
		{
			$new_op = "~";
		}
		$parse_line = $1 . $new_op . $3;
	}
	if ($changed)
	{
		$trans_line[$index] = $parse_line;
		return TRANSLATED;
	}

	# change op [BYTE|WORD|DWORD <stuff>] to op BYTE|WORD|DWORD [<stuff>]
	if ($parse_line =~ m {
		(
			.*
		)
		(
			\[\s*
		)
		(
			(BYTE|WORD|DWORD)
			\s+
		)
		(
			.*\].*
		)
	}ix)
	{
		$trans_line[$index] = $1 . $3 . $2. $5;
		return TRANSLATED;
	}

	# remove FWORD or PWORD associated with LGDT and LIDT
	if ($parse_line =~ s/(.*\b(?:LGDT|LIDT)\b.*)\b(?:PWORD|FWORD)\s+(.*)/$1$2/i)
	{
		$trans_line[$index] = $parse_line;
		return TRANSLATED;
	}

	# change any remaining FWORD or PWORD to DWORD FAR
	if ($parse_line =~ s/(.*)\b(?:PWORD|FWORD)(\s+.*)/$1DWORD FAR$2/i)
	{
		$trans_line[$index] = $parse_line;
		return TRANSLATED;
	}

	# change ST(0-3) to ST0-ST3
	if ($parse_line =~ s/((?:\s|,)ST)\(([0-3])\)/$1$2/gi)
	{
		$trans_line[$index] = $parse_line;
		return TRANSLATED;
	}

#	# change DQ # to DQ #.0 because NASM is really stupid
#	if ($parse_line =~ m/(.*\bDQ\s+)(.*)/i)
#	{
#		my $lead = $1;
#		my $vals = $2;
#		if ($vals =~ s/(\d+)($|[^.])/$1.0$2/g)
#		{
#			$trans_line[$index] = $lead . $vals;
#			return TRANSLATED;
#		}
#	}

	# remove brackets on OUT [#],reg command, because NASM objects
	if ($parse_line =~ m/^(OUT\s+)\[(.*)\](.*)/i)
	{
		$trans_line[$index] = $1 . $2 . $3;
		return TRANSLATED;
	}

	# remove brackets on IN reg,[#] for NASM
	if ($parse_line =~ m/^(IN\s+.*,\s*)\[(.*)\](.*)/i)
	{
		$trans_line[$index] = $1 . $2 . $3;
		return TRANSLATED;
	}

	# the following processing commands can cascade in part or completely

	# convert <group_name> group <group_list> to group <group_name> <group_list>
	if ($parse_line =~ m {
		^
		(
			(
				$valid_symbol_pattern
			)
			(
				\s+
			)
		)
		group\b
		(.*)
	}iox)
	{
		$trans_line[$index] = "GROUP" . $3 . $2 . $4;
		$parse_line = $trans_line[$index];
		$status |= TRANSLATED;
	}

	# convert any group list from comma delimited to space delimited
	#  can be cascaded to from group name conversion, but stops afterwards
	if ($parse_line =~ m/^(GROUP\s+$valid_symbol_pattern\s+)(.*,.*)/io)
	{
		my $group_list = $2;
		$group_list =~ tr/,/ /;
		$trans_line[$index] = $1.$group_list;
		return TRANSLATED;
	}

	# change comparison operators to logical when associated with IF expression
	if ($parse_line =~ m/^(%?IF\s+.*\s)(EQ|NE|GT|GE|LT|LE)(\s.*)/i)
	{
		my $op = uc($2);
		if ($op eq "EQ")
		{
			$op = "=";
		}
		elsif ($op eq "NE")
		{
			$op = "!=";
		}
		elsif ($op eq "GT")
		{
			$op = ">";
		}
		elsif ($op eq "GE")
		{
			$op = ">=";
		}
		elsif ($op eq "LT")
		{
			$op = "<";
		}
		elsif ($op eq "LE")
		{
			$op = "<=";
		}
		$trans_line[$index] = $1 . $op . $3;
		$parse_line = $trans_line[$index];
		$status |= TRANSLATED;
	}

	# change size structure_to structure_size
	if ($parse_line =~ m {
		(.*[^A-Za-z_\@0-9\$\?])
		(size\s+)
		($valid_symbol_pattern)
		(.*)
	}iox)
	{
		my $sizer = uc($3);
		foreach my $key (keys %struc_names)
		{
			if (uc($key) eq $sizer)
			{
				$trans_line[$index] = $1 . $3 . "_size" . $4;
				$parse_line = $trans_line[$index];
				$status |= TRANSLATED;
				last;
			}
		}
	}

	return $status;
}

# ds override is superfluous except when referencing
#  sp, esp, bp, and ebp
#  even then it can be superfluous if multiple 32-bit registers are used, but we won't check for that
# remove ds: if unnecessary because NASM doesn't automatically strip it out like MASM and TASM do
sub strip_extra_ds
{
	my $parse_line = $_[0];
	my $index = $_[1];
	my $status = $_[2];

	if (($parse_line =~ m/(.*)\bds:\b(.*)/i) == 0)
	{
		return $status;
	}

	my $before = $1;
	my $after = $2;
	my $check_field = $after;
	if ($after =~ m/(.*),(.*)/)
	{
		if (defined($1))
		{
			$check_field = $1;
		}
	}
	if ($check_field =~ m/\b(esp|sp|bp|ebp)\b/)
	{
		return $status;
	}

	$trans_line[$index] = $before . $after;
	return TRANSLATED;
}

# multiple line substitution which can end processing
sub process_dup
{
	my $parse_command = $_[0];
	my $index = $_[1];
	my $status = $_[2];

	# look for DUPs of either format
	#  D[x] # DUP (?) or
	#  D[x] # DUP (#)
	#  and translate to res[x] or times command
	#  ensure bytes following DUP are dropped to second line
	if ($parse_command =~ m {
		(.*)
		(
			d(
				[bwd]\s+
			)
		)
		(\S+)
		\s+
		DUP
		\s*
		\(
		\s*
		(\S+.*)
		\)
		(.*)
	}ix)
	{
		my $lead = $1;
		my $type = $2;
		my $size = lc($3);
		my $count = $4;
		my $value = $5;
		my $trail = $6;
		my $multiline = 0;
		
		if ($trail =~ m/^\s*,(.*)/)
		{
			# values following dup, create second line for them
			$multiline = 1;
			$trail = $1;
		}
	
		if ($value eq "?")
		{
			$trans_line[$index] = $lead . "res" . $size . $count;
		}
		else
		{
			$trans_line[$index] =
				$lead . "times" . $separator . $count . $separator . $type . $value;
			if (!$multiline)
			{
				$trans_line[$index] .= $trail;
			}
		}
		if (!$multiline)
		{
			return TRANSLATED;
		}

		# create the two lines
		$multi_line[$multi_index] = $trans_line[$index];
		$trans_line[$index] = $multi_index . ",2";
		$multi_index++;
		$multi_line[$multi_index++] = $separator . $type . $separator . $trail;
		return MULTILINE | TRANSLATED | PROCESSED;
	}
}

# multiple line substitutions which end processing
sub multiline_substs
{
	my $parse_command = $_[0];
	my $index = $_[1];
	my $status = $_[2];

	# remove argument following END, if any
	# add DGROUP line with segment prior to end, if dgroup is flagged
	if ($parse_command =~ m/(^end)(?:$|\s+)/i)
	{
		my $end = $1;
		if (!$dgroup_flag)
		{
			$trans_line[$index] = $end;
			return TRANSLATED;
		}

		# add DGROUP line with collected segments
		$trans_line[$index] = $multi_index;
		my $counter = 0;
		if (scalar(@dgroup_segments))
		{
			$multi_line[$multi_index] = "GROUP" . $separator . "DGROUP";
			foreach my $seg (@dgroup_segments)
			{
				$multi_line[$multi_index] .= $separator . $seg;
			}
			$multi_index++;
			$counter++;
		}
		$multi_line[$multi_index] = $end;
		$counter++;
		$trans_line[$index] .= ",$counter";

		return MULTILINE | TRANSLATED | PROCESSED;
	}

	# <name> macro or macro <name>
	if ($parse_command =~  m {
		^
		(?:
			(?:
				($valid_symbol_pattern)
				\s+
				macro
			)
			|
			(?:
				macro
				\s+
				($valid_symbol_pattern)
			)
		)
		\s*
		(.*)
	}iox)
	{
		my $macro_name;
		if (defined($1))
		{
			$macro_name = $1;
		}
		else
		{
			$macro_name = $2;
		}
		my $arg_list = "";
		if (defined($3))
		{
			$arg_list = $3;
		}

		push	(@macro_type_stack, $macro_name);	# track for passed vars undefine
		push (@macro_type_stack, "%MACRO");	# track for ENDM processing
		@{ $macro_passed_vars{$macro_name} } = ();
		@{ $macro_local_var_store[$#macro_type_stack] } = ();
		while ($arg_list =~ m/($valid_symbol_pattern)/go)
		{
			push (@{ $macro_passed_vars{$macro_name} }, $1);
		}

		$trans_line[$index] = $multi_index;
		my $arg_count = scalar(@{ $macro_passed_vars{$macro_name} });
		$multi_line[$multi_index] =
			"%MACRO" . $separator . $macro_name . $separator . $arg_count;
		if ($arg_count)
		{
			$multi_line[$multi_index] .= "-*";
		}
		$multi_index++;
	
		my $counter = 1;
		foreach my $arg (@{ $macro_passed_vars{$macro_name} })
		{
			$multi_line[$multi_index++] =
				"%DEFINE" . $separator . "%%" . $arg . $separator . "%" . $counter;
			$counter++;
		}
		$trans_line[$index] .= ",$counter";

		return MULTILINE | TRANSLATED | PROCESSED;
	}

	# irp/for macro
	if ($parse_command =~ m {
		^
		(?:IRP|FOR)
		\s+
		($valid_symbol_pattern)
		\s*,\s*
		<\s*
		&?
		($valid_symbol_pattern)
		\s*>
	}iox)
	{
		my $rep_param = $1;
		my $rep_list = $2;
		push (@macro_type_stack, "%REP");	# track for ENDM processing

		foreach my $macro (keys (%macro_passed_vars))
		{
			my $arg_pos = 0;
			foreach my $arg (@{ $macro_passed_vars{$macro} })
			{
				if ($arg eq $rep_list)
				{
					push(@rep_param_stack, $rep_param);
					$trans_line[$index] = $multi_index;
					my $line_count = 0;

					if (!$arg_pos)
					{
						# iterating through args starting at first one
						# pre-position rotate back one to adjust for initial %rep rotate loop
						$multi_line[$multi_index++] = "%ROTATE" . $separator . "-1";
						$line_count++;
					}

					$multi_line[$multi_index] = "%REP" . $separator . "%0";
					if ($arg_pos)
					{
						$multi_line[$multi_index] .= "-$arg_pos";
					}
					$multi_index++;
					$line_count++;
					# iterate through argument list by rotating them through %1
					$multi_line[$multi_index++] = "%ROTATE" . $separator . "1";
					$line_count++;

					$trans_line[$index] .= ",$line_count";
					return MULTILINE | TRANSLATED | PROCESSED;
				}
				$arg_pos++;
			}
		}
		return 0;	# didn't find a match for IRP style supported
	}

	# [.]stack
	if ($parse_command =~ m/^\.?stack\b/i)
	{
		my $stack_size = $parse_command;
		$stack_size =~ s/^\.?(stack)\s*(.*)(\s*.*)/$2/i;
		if (!defined($stack_size) || length($stack_size) < 1)
		{
			$stack_size = "200h";
		}
		my $extra = $3;
		my $res_size = ($memory_model eq "FLAT") ? "d" : "w";
		$trans_line[$index] = $multi_index . ",2";
		my $stack_name = $1;
#		if ($variable_fixing)
#		{
#			$stack_name =~ s/^\$(?=[A-Za-z0-9_\$\@\?])/$variable_fix_character/;
#		}
		if (length($variable_to_fix_characters))
		{
			my ($fixme,$withwhat);
			for (my $i = 0; $i < length($variable_to_fix_characters); $i++)
			{
				$fixme = "\\" . substr($variable_to_fix_characters, $i, 1);
				$withwhat = substr($variable_fixer_characters, $i, 1);
			}
			$stack_name =~ s/^($fixme)(?=[A-Za-z0-9_\$\@\?])/$withwhat/;
		}

		$multi_line[$multi_index++] =
			"SEGMENT" . $separator . $stack_name . $separator . "CLASS=STACK" . $separator . "ALIGN=16";
		$multi_line[$multi_index++] =
			$separator . "res" . $res_size . $separator . $stack_size . $extra;
		push(@segment_name_stack, $stack_name);
		if ($dgroup_flag)
		{
			push (@dgroup_segments, $stack_name);
		}
		return MULTILINE | TRANSLATED | PROCESSED;
	}

	if ($parse_command =~ m {
		^
		(\s*)
		(
			$valid_symbol_pattern
			# ensure not a dotted structure name
			\.?
		)
		(\s*)
		(
			$valid_symbol_pattern
			# ensure not a dotted structure name
			\.?
		)?
		(\s+
		[<\{].*[>\}])?
	}iox)
	{
		# look for structure instance <info> to translate
		foreach my $key (keys %struc_names)
		{
			if ($key ne $2 and (!defined($4) or $key ne $4))
			{
				next;
			}

			# match on structure name
			# save structure instance name, if any
			my $iname;
			if ($key eq $2)
			{
				if (defined($4))
				{
					$iname = $4;
				}
			}
			else
			{
				$iname = $2;
			}
			if (defined($iname))
			{
				$struc_instance{$iname} = $key;
			}

			$trans_line[$index] = $multi_index;
			my $line_counter = 0;
			$multi_line[$multi_index++] = (defined($iname) ? "$2:" : "");
			$line_counter++;
			$multi_line[$multi_index++] = "istruc" . $separator . $key;
			$line_counter++;

			my $member_index = 0;
			my $field = $parse_command;
			while ($field =~ m /
				(<)
				(.*?)
				(,|>)
				(.*)
			/x)
			{
				my $value = $2;
				if (!length($2))
				{
					# no values listed
					last;
				}
				$field = $1 . $4;
				# place field values based on digit
				$multi_line[$multi_index++] =
					$separator . $struc_names{$key}[$member_index++] . $separator . $value;
				$line_counter++;
			}

			$multi_line[$multi_index++] = "iend";
			$line_counter++;
		
			$trans_line[$index] .= ",$line_counter";
			return MULTILINE | TRANSLATED | PROCESSED;
		}
	}

	# multi-argument pushes and pops
	if ($parse_command =~ m {
		^
		(push|pop)
		\s+
		(
			(?!(
				BYTE|WORD|DWORD
			))
			[A-Za-z0-9]+
			|
			\[.*?\]
		)+
		\s+
		(
			(?!(
				BYTE|WORD|DWORD
			))
			[A-Za-z0-9]+
			|
			\[.*?\]
		)+
	}ix)
	{
		my $pushpop = $1;
		$trans_line[$index] = $multi_index;
		my $count = 0;
		while ($parse_command =~ m/([A-Za-z0-9]+|\[.*?\])/g)
		{
			if (!$count)
			{
				next;
			}
			$multi_line[$multi_index++] = $pushpop . $separator . $1;
		}
		continue
		{
			$count++;
		}
		$count--;
		$trans_line[$index] .= ",$count";
		return MULTILINE | TRANSLATED | PROCESSED;
	}

	# purge macros
	if ($parse_command =~ m/^PURGE\s+(.*)/i)
	{
		my $macro_list = $1;
		my $count = 0;
		$trans_line[$index] = $multi_index;
		while ($macro_list =~ m/($valid_symbol_pattern)/go)
		{
			$multi_line[$multi_index++] = "%UNDEF" . $separator . $1;
			$count++;
		}
		$trans_line[$index] .= ",$count";
		return MULTILINE | TRANSLATED | PROCESSED;
	}

	# look for label that needs start address designation
	if ($start_label &&
		$parse_command =~ m/^($valid_symbol_pattern)\s*:\s*$/o)
	{
		if ($1 eq $start_label)
		{
			$trans_line[$index] = $multi_index . ",2";
			$multi_line[$multi_index++] = "..start:";
			$multi_line[$multi_index++] = $parse_command;
			return MULTILINE | TRANSLATED | PROCESSED;
		}
	}

	return $status;
}

# commands which do not end translation (line not flagged as processed)

# note that this parses structure Dx's to res[x]'s even if initialized
# also note that this changes structure <member_name> Dx's to .<member_name> Dx's
sub command_dx
{
	my $parse_command = $_[0];
	my $index = $_[1];

	my $valid = $parse_command =~ m {
		^
		(
			(
				$valid_symbol_pattern
			)
			\s+
		)?
		d
		(b|w|d)
		\s+
		\(?
		\s*
		(.*)
	}iox;
	if (!$valid)
	{
		return 0;
	}

	my $leading = defined($1) ? $1 : "";
	my $var = defined($2) ? $2 : "";
	my $dtype = lc($3);
	my $dbparam = $4;
	if (!$parsing_struc_flag)
	{
		if (substr($dbparam, 0, 1) ne "?")
		{
			# don't change Dx non-? if not in structure
			return 0;
		}
		else
		{
			$trans_line[$index] = $leading . "res" . $dtype . $separator . "1";
		}
	}
	else	# parsing a structure
	{
		$struc_names{$current_struc_name}[$struc_member_index++] = "d$dtype";

		# if there is a DUP in the field, try to grab the count for the res[x]
		my $dup_count = "";
		if ($dbparam =~ m/\s*(.*)\bDUP\b.*/i)
		{
			$dup_count = $1;
		}

		my $count = "1";
		if (length($dup_count))
		{
			$count = $dup_count;
		}
		if (length($var))
		{
			$trans_line[$index] =
				"." . $var . ":" . $separator . "res" . $dtype . $separator . $count;
		}
		else
		{
			$trans_line[$index] = $leading . "res" . $dtype . $separator . $count;
		}
	}

	# do not flag as processed, as more processing may be necessary
	return TRANSLATED;
}

sub command_extrn
{
	my $parse_command = $_[0];
	my $index = $_[1];

	if ($parse_command =~ m/^extrn\s+$valid_symbol_pattern/io)
	{
		$parse_command =~ s/^(extrn(\s+))(.*)/EXTERN$2$3/i;
		$parse_command =~
			s/($valid_symbol_pattern)(?:\s*:\s*([A-Za-z]+))?/$1/go;
	}

	$trans_line[$index] = $parse_command;
	return TRANSLATED;
}

# variable name and type have already been extracted in initial variable scan
#  just make a simple ':' label
sub command_label
{
	my $parse_command = $_[0];
	my $index = $_[1];

	# handle both masm and ideal mode with leading or following label name
	$parse_command =~ m {
		^
		(
			(
				($valid_symbol_pattern)
				\s+
				LABEL
			)
			|
			(
				LABEL
				\s+
				($valid_symbol_pattern)
			)
		)
		\s+
		(BYTE|WORD|DWORD|NEAR|FAR|FWORD|TBYTE|TWORD)
	}iox;

	my $label;
	if (defined($3))
	{
		$label = $3;
	}
	else
	{
		$label = $5;
	}
#	if ($variable_fixing)
#	{
#		$label =~ s/^\$(?=[A-Za-z0-9_\$\@\?])/$variable_fix_character/;
#	}
	if (length($variable_to_fix_characters))
	{
		my ($fixme,$withwhat);
		for (my $i = 0; $i < length($variable_to_fix_characters); $i++)
		{
			$fixme = "\\" . substr($variable_to_fix_characters, $i, 1);
			$withwhat = substr($variable_fixer_characters, $i, 1);
		}
		$label =~ s/^($fixme)(?=[A-Za-z0-9_\$\@\?])/$withwhat/;
	}
	$trans_line[$index] = "$label:";
	return TRANSLATED;
}

sub command_proc
{
	my $parse_command = $_[0];
	my $index = $_[1];

	$inside_proc_flag = $parse_command =~ m {
		^
		(?:
			(?:
				(?!(?:
					NEAR|FAR
				))
				($valid_symbol_pattern)
				\s+
				PROC
			)
			|
			(?:
				PROC
				\s+
				(?!(?:
					NEAR|FAR
				))
				($valid_symbol_pattern)
			)
		)
		(?:
			\s+
			(NEAR|FAR)
		)?
	}iox;

	my $proc_name;
	if (defined($1))
	{
		$proc_name = $1;
	}
	elsif (defined($2))
	{
		$proc_name = $2;
	}

	if (defined($3))
	{
		$proc_size = uc($3);
	}
	else
	{
		$proc_size = 0;
	}

	if (defined($proc_name))
	{
		$last_proc_name = $proc_name;
#		if ($variable_fixing)
#		{
#			$proc_name =~ s/^\$(?=[A-Za-z0-9_\$\@\?])/$variable_fix_character/;
#		}
		if (length($variable_to_fix_characters))
		{
			my ($fixme,$withwhat);
			for (my $i = 0; $i < length($variable_to_fix_characters); $i++)
			{
				$fixme = "\\" . substr($variable_to_fix_characters, $i, 1);
				$withwhat = substr($variable_fixer_characters, $i, 1);
			}
			$proc_name =~ s/^($fixme)(?=[A-Za-z0-9_\$\@\?])/$withwhat/;
		}
		$trans_line[$index] = $proc_name . ":";
		return TRANSLATED;
	}
	else
	{
		$last_proc_name = "";
	}

	# unnamed proc has no substitution
	return REMOVED | PROCESSED;
}

sub command_public
{
	my $parse_command = $_[0];
	my $index = $_[1];

	$parse_command =~ m/^public\s+(.*)/i;
	my $symbols = $1;
	$trans_line[$index] = "GLOBAL" . $separator . $symbols;
	return TRANSLATED;
}

sub command_seg
{
	my $parse_command = $_[0];
	my $index = $_[1];

	if ($parse_command =~ m/(.*)(\bSEG\b\s*)($valid_symbol_pattern)(.*)/io)
	{
		my $seg_name = uc($3);
		foreach my $segs (@segment_name_stack)
		{
			if (uc($segs) eq $seg_name)
			{
				$trans_line[$index] = $1 . $3 . $4;
				return TRANSLATED;
			}
		}
	}

	return 0;
}

sub command_struc_leading
{
	my $parse_command = $_[0];
	my $index = $_[1];

	$parse_command =~ m/^struc\s+(\S+)/i;
	my $struc_name = $1;
	return command_struc($struc_name, $index)
}

sub command_struc_trailing
{
	my $parse_command = $_[0];
	my $index = $_[1];

	$parse_command =~ m/^(\S+)\s+struc\b/i;
	my $struc_name = $1;
	return command_struc($struc_name, $index)
}

sub command_struc
{
	my $struc_name = $_[0];
	my $index = $_[1];

	$parsing_struc_flag = 1;
	$current_struc_name = $struc_name;
	$struc_member_index = 0;
	$trans_line[$index] = "struc" . $separator . $struc_name;

	return TRANSLATED;
}

sub command_perc_prefix
{
	my $parse_command = $_[0];
	my $index = $_[1];

	$parse_command =~ s/^(\S+)/%$1/i;
	$trans_line[$index] = $parse_command;

	return TRANSLATED;
}

sub subst_tbyte
{
	my $parse_command = $_[0];
	my $index = $_[1];

	$parse_command =~ s/\btbyte\b/TWORD/i;
	$trans_line[$index] = $parse_command;

	return TRANSLATED;
}

sub subst_xmmword
{
	my $parse_command = $_[0];
	my $index = $_[1];

	$parse_command =~ s/\bxmmword\b/OWORD/i;
	$trans_line[$index] = $parse_command;

	return TRANSLATED;
}

sub command_ife
{
	my $parse_command = $_[0];
	my $index = $_[1];

	$parse_command =~ s/^(if)e(\s+)(.*)/%ifndef$2$3/i;
	$trans_line[$index] = $parse_command;

	return TRANSLATED;
}

sub command_ifb
{
	my $parse_command = $_[0];
	my $index = $_[1];

	$parse_command =~ s/^ifb(\s+)<(.*)>/%ifnnum$1$2/i;
	$trans_line[$index] = $parse_command;
	return TRANSLATED;
}

sub command_ifnb
{
	my $parse_command = $_[0];
	my $index = $_[1];

	$parse_command =~ s/^ifnb(\s+)<(.*)>/%ifnum$1$2/i;
	$trans_line[$index] = $parse_command;
	return TRANSLATED;
}

sub command_elseif
{
	my $parse_command = $_[0];
	my $index = $_[1];

	$parse_command =~ s/^(el)se(if)/%$1$2/i;
	$trans_line[$index] = $parse_command;
	return TRANSLATED;
}

# commands which end translation (line flagged as processed)

sub command_code
{
	my $parse_command = $_[0];
	my $index = $_[1];

	$parse_command =~ m/^\.(code)(.*)/i;
	$trans_line[$index] = "SEGMENT" . $separator . $1 . $separator . "CLASS=CODE" . $separator . "ALIGN=2" . $2;
	push(@segment_name_stack, $1);
	if ($dgroup_flag)
	{
		push (@dgroup_segments, $1);
	}
	return TRANSLATED | PROCESSED;
}

sub command_comment
{
	my $parse_command = $_[0];
#	my $index = $_[1];

	$parse_command =~ m/^comment\s+(\S).*/i;
	$comment_delimiter = $1;
	$comment_flag = 1;
	return COMMENTED | PROCESSED;
}

sub command_cpu
{
	my $parse_command = $_[0];
	my $index = $_[1];

	$parse_command =~ m/^(?:\.|P)([1-6])86P?(.*)/i;
	$cpu_level = $1;
	$trans_line[$index] = "CPU" . $separator . $cpu_level . "86" . $2;
	return TRANSLATED | PROCESSED;
}

sub command_data_const
{
	my $parse_command = $_[0];
	my $index = $_[1];

	my $extra = $parse_command;
	$parse_command =~ m/^\.?(data|const)(.*)/i;
	$trans_line[$index] =
		"SEGMENT" . $separator . $1 . $separator . "CLASS=DATA" . $separator . "ALIGN=2" . $2;
	push(@segment_name_stack, uc($1));
	if ($dgroup_flag)
	{
		push (@dgroup_segments, $1);
	}
	return TRANSLATED | PROCESSED;
}

sub command_endm
{
#	my $parse_command = $_[0];
	my $index = $_[1];

	if (scalar(@macro_type_stack))
	{
		my $type = pop(@macro_type_stack);
		pop(@macro_local_var_store);
		my $valid = 0;
		if ($type eq "%MACRO")
		{
			# clear passed variable tracking for this macro
			my $macro_name = pop(@macro_type_stack);
			undef(@{ $macro_passed_vars{$macro_name} });

			$trans_line[$index] = "%ENDMACRO";
			$valid = 1;
		}
		elsif ($type eq "%REP")
		{
			pop(@rep_param_stack);	# clear the parameter tracking for this rep
			$trans_line[$index] = "%ENDREP";
			$valid = 1;
		}

		if ($valid)
		{
			return TRANSLATED | PROCESSED;
		}
	}

	return REMOVED | PROCESSED;
}

sub command_endp
{
	my $parse_command = $_[0];
#	my $index = $_[1];

	$last_proc_name = "";
	$inside_proc_flag =
		($parse_command =~ m/\bendp\b/i) ? 0 : $inside_proc_flag;
	return REMOVED | PROCESSED;
}

sub command_even
{
#	my $parse_command = $_[0];
	my $index = $_[1];

	$trans_line[$index] = "ALIGN" . $separator . "2";
	return TRANSLATED | PROCESSED;
}

sub command_include
{
	my $parse_command = $_[0];
	my $index = $_[1];

	if ($parse_command =~ m/^include(\s+)(\")?([^ \t\"]+)(\")?/i)
	{
		$trans_line[$index] = "%include" . $1 . "\"" . $3 . "\"";
		return TRANSLATED | PROCESSED;
	}
}

sub command_model
{
	my $parse_command = $_[0];
	my $index = $_[1];

	# declaring .386+ before model defaults to USE32 code
	if ($cpu_level >= 3)
	{
		$use32 = 1;
	}

	$parse_command =~ m/^\.model\s+(\S+)/i;
	$memory_model = uc($1);
	$dgroup_flag = 1;
#	$trans_line[$index] = "GROUP" . $separator . "DGROUP" . $separator . "DATA CONST";
#	return TRANSLATED | PROCESSED;
	return REMOVED |PROCESSED
}

sub command_segment
{
	my $parse_command = $_[0];
	my $index = $_[1];

	my @seg_stuff;
	while ($parse_command =~ m/(\S+)/g)
	{
		push (@seg_stuff, $1);
	}

	my $element = 1;
	my $seg_name;
	my $segment_text = "SEGMENT";
	if (uc($seg_name = $seg_stuff[0]) eq $segment_text)
	{
		$seg_name = "";
	}
	else
	{
		$element++;
		$segment_text .= $separator;
	}
#	if ($variable_fixing)
#	{
#		$seg_name =~ s/^\$(?=[A-Za-z0-9_\$\@\?])/$variable_fix_character/;
#	}
	if (length($variable_to_fix_characters))
	{
		my ($fixme,$withwhat);
		for (my $i = 0; $i < length($variable_to_fix_characters); $i++)
		{
			$fixme = "\\" . substr($variable_to_fix_characters, $i, 1);
			$withwhat = substr($variable_fixer_characters, $i, 1);
		}
		$seg_name =~ s/^($fixme)(?=[A-Za-z0-9_\$\@\?])/$withwhat/;
	}
	if ($seg_name)
	{
		push(@segment_name_stack, $seg_name);
	}

	$trans_line[$index] = $segment_text . $seg_name;
	my $stack_class;
	my $gave_align = 0;
	my $align = "ALIGN=";
	foreach my $seg_stuff (@seg_stuff[$element..$#seg_stuff])
	{
		my $upper = uc($seg_stuff);
		if ($upper eq "BYTE")
		{
			$seg_stuff = $align . "1";
			$gave_align = 1;
		}
		elsif ($upper eq "WORD")
		{
			$seg_stuff = $align . "1";
			$gave_align = 1;
		}
		elsif ($upper eq "DWORD")
		{
			$seg_stuff = $align . "4";
			$gave_align = 1;
		}
		elsif ($upper eq "PARA")
		{
			$seg_stuff = $align . "16";
			$gave_align = 1;
		}
		elsif ((substr($upper, 0, 1) eq "\'") && (length($upper) > 2))
		{
			$seg_stuff =~ s/.(.*)./CLASS=$1/;
			if (uc($1) eq "STACK")
			{
				$stack_class = 1;
			}
#			if ($variable_fixing)
#			{
#				$seg_stuff =~ s/(?<==)\$(?=[A-Za-z0-9_\$\@\?])/$variable_fix_character/;
#			}
			if (length($variable_to_fix_characters))
			{
				my ($fixme,$withwhat);
				for (my $i = 0; $i < length($variable_to_fix_characters); $i++)
				{
					$fixme = "\\" . substr($variable_to_fix_characters, $i, 1);
					$withwhat = substr($variable_fixer_characters, $i, 1);
				}
				$seg_stuff =~ s/(?<==)($fixme)(?=[A-Za-z0-9_\$\@\?])/$withwhat/;
			}
		}
		elsif ($upper eq "USE16")
		{
			$use32 = 0;
		}
		elsif (($upper eq "USE32") || ($upper eq "FLAT"))
		{
			$use32 = 1;
		}

		$trans_line[$index] .= $separator . $seg_stuff;
	}

	# give stack class segments a para alignment if not listed
	if ($stack_class && !$gave_align)
	{
		$trans_line[$index] .= $separator . "ALIGN=16";
	}

	return TRANSLATED | PROCESSED;
}

sub command_ignore
{
	return IGNORED | REMOVED | PROCESSED;
}

# output
sub output_translation
{
	if ($verbosity)
	{
		print "Writing translated file: $outfile\n";
	}
	my $line_count = @source;
	for (my $i = 0; $i < $line_count; $i++)
	{
		if (!($status[$i] & PROCESSED))
		{
			print OUTFILE $source[$i];
		}
		elsif (!($status[$i] & REMOVED))
		{
			if ($status[$i] & COMMENTED)
			{
				print OUTFILE ";";
				print OUTFILE $parse_lead[$i];
				print OUTFILE $parse_command[$i];
				print OUTFILE $parse_trail[$i];
				print OUTFILE $parse_comment[$i];
				print OUTFILE $parse_eol[$i];
			}
			elsif ($status[$i] & MULTILINE)
			{
				my $start_index = $trans_line[$i];
				$start_index =~ s/(\d+).*/$1/;
				my $count = $trans_line[$i];
				$count =~ s/\d+,(\d+)/$1/;
				for (my $j = 0; $j < $count; $j++)
				{
					print OUTFILE $parse_lead[$i];
					print OUTFILE $multi_line[$start_index + $j];
					if ($j == 0)
					{
						# only print trail and comment on first of multiple lines
						print OUTFILE $parse_trail[$i];
						print OUTFILE $parse_comment[$i];
					}
					print OUTFILE $parse_eol[$i];
				}
			}
			else
			{
				print OUTFILE $parse_lead[$i];
				print OUTFILE $trans_line[$i];
				print OUTFILE $parse_trail[$i];
				print OUTFILE $parse_comment[$i];
				print OUTFILE $parse_eol[$i];
			}
		}
	}
}
