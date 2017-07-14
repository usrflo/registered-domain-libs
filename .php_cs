<?php

return PhpCsFixer\Config::create()
    ->setUsingCache(true)
    ->setRules(array(
        'array_syntax' => ['syntax' => 'long'],
        'combine_consecutive_unsets' => true,
        'list_syntax' => ['syntax' => 'long'],
        'no_closing_tag' => true,
        'no_extra_consecutive_blank_lines' => ['break', 'continue', 'extra', 'return', 'throw', 'use', 'parenthesis_brace_block', 'square_brace_block', 'curly_brace_block'],
        'no_short_echo_tag' => true,
        'no_useless_return' => true,
	'no_whitespace_in_blank_line' => true,
        'semicolon_after_instruction' => true,
    ))
    ->setFinder(
        PhpCsFixer\Finder::create()
            ->in(__DIR__)
            ->notPath('PHP/effectiveTLDs.inc.php')
    )
;
